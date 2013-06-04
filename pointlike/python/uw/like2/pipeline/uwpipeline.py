"""
task UWpipeline Interface to the ISOC PipelineII

$Header$
"""
import os, argparse, logging, datetime, subprocess
import numpy as np
from uw.like2.pipeline import check_data
from uw.like2.pipeline import pipeline_job
from uw.like2.pipeline import check_converge
from uw.like2.pipeline import diagnostic_plots, pipe
from uw.like2.pipeline import processor

class StartStream(object):
    """ setup, start a stream """
    def main(self, args):
        pipeline='/afs/slac/g/glast/ground/bin/pipeline -m PROD createStream '
        for stage in args.stage:
            # note set job_list to the name of a local file in the pointlike folder -- needs better encapsulation
            job_list = args.job_list
            if job_list is None or job_list=='None':
                job_list = stagenames[stage]['job_list']
            cmd=pipeline+' -D "stage=%s, SKYMODEL_SUBDIR=%s, job_list=%s" UWpipeline' \
                %(stage, args.skymodel, job_list)
            print '-->' , cmd
            if not args.test:
                os.system(cmd)
                #print subprocess.check_output(cmd)

class Summary(object):
    def get_stage(self, args):
        stagelist = args.stage[0] 
        t = stagelist.split(':',1)
        if len(t)==2:
            stage, nextstage = t 
        else: stage,nextstage = t[0], None
        return stage
    def main(self, args):
        stage = self.get_stage(args)
        kw = stagenames[stage].get('sum', None)
        if kw is not None:
            diagnostic_plots.main(kw.split())
            
class JobProc(Summary):
    """ process args for running pipeline jobs"""
    def main(self, args):
        stage = self.get_stage(args)
        setup = stagenames[stage].setup()
        pipeline_job.main(setup)
  
class CheckJobs(Summary):
    """ process args for the check_jobs step"""
    def main(self, args):
        from uw.like2.pub import healpix_map
        stage = self.get_stage(args)
        # do something instead
        check_converge.main(args)

class CheckData(Summary):
    def main(self, args):
        stage = self.get_stage(args)
        check_data.main(args)
 
class Proc(dict):
    def __init__(self, run, help='', **kwargs):
        """ run: class or module -- must have main function """
        super(Proc, self).__init__(self,  help=help, **kwargs)
        self.run = run
        self['help']=help

    def __call__(self, args):
        self.run.main(args)
 
procnames = dict(
    # proc names (except for start) generated by the UWpipeline task as stream executes
    # start actually launches a stream
    start      = Proc(StartStream(), help='start a stream: assumed if no proc is specified'),
    check_data = Proc(CheckData(),     help='check that required data files are present'),
    job_proc   = Proc(JobProc(),     help='run a parallel pipeline job'),
    check_jobs = Proc(CheckJobs(),   help='check for convergence, combine results, possibly submit new stream'),
    summary_plots= Proc(Summary(),   help='Generate summary plots, depending on stage name'),
    )
proc_help = '\nproc names\n\t' \
    +'\n\t'.join(['%-15s: %s' % (key, procnames[key]['help'])  for key in sorted(procnames.keys())])
    
class Stage(dict):
    def __init__(self, proc, pars={}, job_list='joblist.txt', help='', **kwargs):
        super(Stage,self).__init__(proc=proc, pars=pars, help=help, **kwargs)
        self['help']=help
        self['job_list']=job_list
    def setup(self):
        return self['proc'](**self['pars'])

stagenames = dict(
    # List of possible stages, with proc to run, parameters for it,  summary string
    # list is partly recognized by check_converge.py, TODO to incoprorate it here, especially the part that may start a new stream
    create      =  Stage(pipe.Create,  sum='environment counts menu', help='Create a new skymodel, follow with update_full',),
    update_full =  Stage(pipe.Update, dict( dampen=1.0,),sum='counts',help='perform update' ),
    update      =  Stage(pipe.Update, dict( dampen=0.5,),sum='counts',help='perform update' ),
    update_beta =  Stage(pipe.Update, dict( dampen=1.0, fix_beta=True),sum='counts',help='perform update', ),
    update_pivot=  Stage(pipe.Update, dict( dampen=1.0, repivot=True), sum='counts',help='update pivot', ), 
    update_only =  Stage(pipe.Update, dict( dampen=1.0), sum='counts sources', help='update, no additional stage', ), 
    finish      =  Stage(pipe.Finish,  sum='sources',help='perform localization', ),
    tables      =  Stage(pipe.Tables,  sum='hptables', job_list='joblist8.txt', help='create HEALPix tables: ts kde counts', ),
    sedinfo     =  Stage(pipe.Update, dict( processor='processor.full_sed_processor',sedfig_dir='"sedfig"',), sum='fb',
                            help='process SED information' ),
    diffuse     =  Stage(pipe.Update, dict( processor='processor.roi_refit_processor'), sum='galspect', ),
    isodiffuse  =  Stage(pipe.Update, dict( processor='processor.iso_refit_processor'), sum='isospect', ),
    limb        =  Stage(pipe.Update, dict( processor='processor.limb_processor'),     sum='limb_refit', help='Refit the limb component, usually fixed' ),
    sunmoon     =  Stage(pipe.Update, dict( processor='processor.sunmoon_processor'), sum='sunmoon_refit', help='Refit the SunMoon component, usually fixed' ),
    fluxcorr    =  Stage(pipe.Update, dict( processor='processor.flux_correlations'), sum='fluxcorr', ),
    fluxcorrgal =  Stage(pipe.Update, dict( processor='processor.flux_correlations'), sum='flxcorriso', ),
    fluxcorriso =  Stage(pipe.Update, dict( processor='processor.flux_correlations(diffuse="iso*", fluxcorr="fluxcorriso")'), ),
    pulsar_table=  Stage(pipe.PulsarLimitTables,),
    localize    =  Stage(pipe.Update, dict( processor='processor.localize(emin=1000.)'), help='localize with energy cut' ),
    seedcheck   =  Stage(pipe.Finish, dict( processor='processor.check_seeds(prefix="SEED")',auxcat="seeds.txt"), 
                                                                       help='Evaluate a set of seeds: fit, localize with position update, fit again'),
    seedcheck_MRF =  Stage(pipe.Finish, dict( processor='processor.check_seeds(prefix="MRF")', auxcat="4years_SeedSources-MRF.txt"), help='refit MRF seeds'),
    seedcheck_PGW =  Stage(pipe.Finish, dict( processor='processor.check_seeds(prefix="PGW")', auxcat="4years_SeedSources-PGW.txt"), help='refit PGW seeds'),
    pseedcheck  =  Stage(pipe.Finish, dict( processor='processor.check_seeds(prefix="PSEED")',auxcat="pseeds.txt"), help='refit pulsar seeds'),
    fglcheck    =  Stage(pipe.Finish, dict( processor='processor.check_seeds(prefix="2FGL")',auxcat="2fgl_lost.csv"), help='check 2FGL'),
    pulsar_detection=Stage(pipe.PulsarDetection, job_list='joblist8.txt', sum='pts', help='Create ts tables for pulsar detection'),
    gtlike_check=  Stage(pipe.Finish, dict(processor='processor.gtlike_compare()',), sum='gtlike_comparison', help='Compare with gtlike analysis of same sources'),
    uw_compare =  Stage(pipe.Finish, dict(processor='processor.UW_compare(other="uw25")',), sum='uw_comparison', help='Compare with another UW model'),
) 
keys = stagenames.keys()
stage_help = '\nstage name, or sequential stages separaged by ":" names are\n\t' \
    +  '\n\t'.join(['%-15s: %s' % (key,stagenames[key]['help'])  for key in sorted(stagenames.keys())])

def check_environment(args):
    if 'SKYMODEL_SUBDIR' not in os.environ:
        os.environ['SKYMODEL_SUBDIR'] = os.getcwd()
    else:
        skymodel = os.environ['SKYMODEL_SUBDIR']
        print 'skymodel:' , skymodel
        skymodel = skymodel.replace('/a/wain025/g.glast.u55/', '/afs/slac/g/glast/groups/')
        assert os.path.exists(skymodel), 'Bad path for skymodel folder: %s' %skymodel
        os.chdir(skymodel)
    cwd = os.getcwd()
    assert os.path.exists('config.txt'), 'expect this folder (%s) to have a file config.txt'%cwd
    m = cwd.find('skymodels')
    assert m>0, 'did not find "skymodels" in path to cwd, which is %s' %cwd
    if args.stage[0] is None :
        pass #    raise Exception( 'No stage specified: either command line or os.environ')
    else:
        os.environ['stage']=args.stage[0]

    # add these to the Namespace object for convenience
    args.__dict__.update(skymodel=cwd, pointlike_dir=cwd[:m])

def check_names(stage, proc):
    if len(stage)==0:
        if proc is  None:
            raise Exception('No proc or stage argement specified')
        if proc not in procnames:
            raise Exception('proc name "%s" not in list %s' % (proc, procnames,keys()))
        return
    if stage[0] is None: 
        raise Exception('no stage specified')
    for s in stage:
        for t in s.split(':'):
            if t not in keys:
                raise Exception('"%s" not found in possible stage names, %s' %(t, keys))

def main( args ):
    check_environment(args)
    check_names(args.stage, args.proc)
    proc = args.proc
    #tee = processor.OutputTee('summary_log.txt')
    print '\n'+ str(datetime.datetime.today())[:16]
    print '--> %s for %s'%(proc, args.stage)
    procnames[proc](args)
    #tee.close()

if __name__=='__main__':
    parser = argparse.ArgumentParser(formatter_class=argparse.RawDescriptionHelpFormatter,
            description=""" start a UWpipeline stream, or run a UWpipeline proc; \n
    """+stage_help+proc_help+"""
    \nExamples: 
        uwpipeline create
        uwpipeline finish -p summary_plots
    """)
    parser.add_argument('stage', nargs='*', default=[os.environ.get('stage', None)], help='stage name, default "%(default)s"')
    parser.add_argument('-p', '--proc', default=os.environ.get('PIPELINE_PROCESS', 'start'), 
            help='proc name,  default: "%(default)s"')
    parser.add_argument('--job_list', default=os.environ.get('job_list', None), help='file used to allocate jobs, default "%(default)s"')
    parser.add_argument('--stream', default=os.environ.get('PIPELINE_STREAM', -1), help='pipeline stream number, default %(default)s')
    parser.add_argument('--test', action='store_true', help='Do not run' )
    parser.add_argument('--processor',  help='specify the processor' )
    args = parser.parse_args()
    main(args)
    
 
