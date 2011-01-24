"""
Manage creation of DeepZoom images

Wraps the basic functionality from the (modified) deepzoom package 
$Header$

Author: T.Burnett <tburnett@uw.edu>
"""
import os, sys, glob, exceptions
import optparse
from . import deepzoom 

class InvalidParameter(exceptions.Exception):
    pass

class MakeCollection(object):
    """
    Create a DeepZoom collection, converting a set of images.
    Usage: 
        mc = dz_collection.MakeCollection('uw93/combined', '/phys/users/tburnett/pivot/24M_uw93')
        mc.convert()
        mc.collect()
        
    Note that the convert step time is order(number of images) and can be parallized by passing an
        optional IPython.kernel.MultiEngineClient object
    """
    def __init__(self, infolder, outfolder,
            imagetype = 'jpg', 
            collection_name='dzc', 
        ):
        """ 
        infolder   :  path to a folder containing a bunch of images (jpg by default) to convert
        outfolder  :  where to set up the collection
        keyword parameters
            collection_name: ['dzc'] name to apply to the deep zoom collection: 
                            will be the name of a xml file and the folder name containing the DZ images
            imagetype      : ['jpg'] graphics type, perhaps 'png'
        """
        self.infolder, self.outfolder = infolder, outfolder
        self.collection_name = collection_name 
        self.files = glob.glob('%s/*.%s' % (infolder, imagetype)); n=len(self.files)
        self.type = imagetype
        self.files.sort() 
        if n ==0: raise InvalidParameter('no %s files found in folder "%s"' % (imagetype,infolder))
        print 'Will convert %d %s images from %s, collection %s' \
                % (n, imagetype, infolder,  collection_name)
        if not os.path.exists(outfolder): os.mkdir(outfolder)
        self.creator =deepzoom.ImageCreator().create 
    
    def __call__(self, x):
        """ converts  single file
        """
        outfile = os.path.join(self.outfolder,
                self.collection_name, os.path.split(x)[1].replace('.'+self.type,'.xml'))
        self.creator(x, outfile)

    def convert(self, mec=None):
        """ convert all found files
        
        mec : None or a  IPython.kernel.client.MultiEngineClient instance
            Must have started the engines, perhaps with: ipcluster local -xy -n 8 &
            (when done, can kill them with mec.kill(True) )
        """
        if mec is None:
            map(self, self.files)
            return
        ids = mec.get_ids()
        if len(ids)==0: raise InvalidParameter('no MEC ids')
        # Start duplicates of this object in each engine, assume same filesystem
        mec.execute('from uw.pipeline.pub import dz_collection') 
        mec.execute('import os; os.chdir("%s")'%os.getcwd()) 
        mec.execute('mc = dz_collection.MakeCollection("%s","%s")'  %(self.infolder, self.outfolder))
        # allocate alternate images for each engine to convert
        pending =[mec.execute('map(mc,mc.files[%d::%d])' %(i, len(ids)), i, block=False) for i in ids]
        # wait for all to finish
        results = [p.get_result(block=True) for p in pending] 
        return results
        
    def collect(self):
        """ make a collection using the files generated by the convert step
        """
        # do this in the output folder so that the file paths in the xml file will be relative
        cwd = os.getcwd() 
        os.chdir(self.outfolder)
        images = glob.glob(os.path.join(self.collection_name, '*.xml'))
        if len(images)==0: 
            print 'Found no DeepZoom images in %s, no collection made.'\
                    %os.path.join(self.outfolder,self.collection_name)
        else:
            print 'Creating collection with %d images' %len(images)
            deepzoom.CollectionCreator().create(images, self.collection_name)
        os.chdir(cwd)     

###################################################################################
def main():   
    parser = optparse.OptionParser(
        usage="""Usage: %prog [options] infolder outfolder
        
        infolder: input folder containing .jpg files
        outfolder: output folder to contain collection
        """)

    # implementing this means dependence on IPython
    parser.add_option("-m", "--mec", dest="mec", default=False,
                  help="Use IPython MultiEngineClient")
    
    parser.add_option("-n", "--name", dest="name", default='dzc', 
                  help="collection name, default 'dzc'")
    (options, args) = parser.parse_args()

    if len(args)!=2:
        parser.print_help()
        sys.exit(1)
    infolder,outfolder = map(
        lambda d:os.path.abspath(os.path.expanduser(os.path.expandvars(d))),args
        )

    for folder in (infolder, outfolder):
        if not os.path.exists(folder):
            print "Folder %s not found" % folder
            sys.exit(1)
    mec = None 
    if options.mec:
        from IPython.kernel.client import MultiEngineClient
        mec = MultiEngineClient()
        
    print infolder, '-->', outfolder
    mc = MakeCollection(infolder, outfolder, collection_name=options.name)
    mc.convert(mec)
    mc.collect()

if __name__ == "__main__":
    main()    