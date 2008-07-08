#! /usr/bin/env python
#
#=====================================================================#
#                    LAT source association pipeline
# ------------------------------------------------------------------- #
# Author: $Author$
# Revision: $Revision$
# Date: $Date$
#=====================================================================#

import os                   # operating system module
import glob                 # filename access
import sys                  # system
import pyfits               # FITS file access
import commands             # command execution


#=============#
# Set globals #
#=============#
col_prefix = "@"


#=========================#
# Set gtsrcid job command #
#=========================#
def set_command(task, parameters):
	"""
	Set job command.
	"""
	# Set task name
	cmd = task
	
	# Add parameters
	for key, value in parameters.iteritems():
		if str(value) != "_skip":
			cmd += " " + key + "='" + str(value) + "'"
	
	# Return command
	return cmd


#=============#
# Run gtsrcid #
#=============#
def run_command(cmd):
	"""
	Run job command.
	"""
	# Run job
	#print cmd
	error, result = commands.getstatusoutput(cmd)
	
	# Return error and result
	return error, result


#=================================================#
# Determine source class catalogue directory path #
#=================================================#
def get_cat_path():
	"""
	Determine counterpart catalogues directory path from 'GLAST_CAT'
	or 'EXTFILESSYS'.
	
	Returns:
	 Source class catalogue directory path
	"""
	
	# Get path from environment variable
	try:
		path = os.environ['GLAST_CAT']
	except KeyError:
		path = os.path.join(os.environ['EXTFILESSYS'],'catalogues')
	
	# Return directory path
	return path


#=====================================================#
# Set source class parameters from module information #
#=====================================================#
def set_pars(module, chatter=False):
	"""
	Set the source class parameters using information in python modules.
	
	Arguments:
	 module    Python module from which the information is extracted
	 chatter=  Chatter flag
	Returns:
	 Parameter dictionary
	"""
	# Set v2r0p0 default parameters
	par = {'srcCatName': "", \
	       'srcCatPrefix': "LAT", \
	       'srcCatQty': "*", \
	       'srcPosError' : 0.0, \
	       'cptCatName': "", \
	       'cptCatPrefix': "CPT", \
	       'cptCatQty': "*", \
	       'cptPosError' : 1.0/3600.0, \
	       'cptDensFile': '', \
	       'outCatName': "result.fits", \
	       'outCatQty01': "", \
	       'outCatQty02': "", \
	       'outCatQty03': "", \
	       'outCatQty04': "", \
	       'outCatQty05': "", \
	       'outCatQty06': "", \
	       'outCatQty07': "", \
	       'outCatQty08': "", \
	       'outCatQty09': "", \
	       'probMethod': "PROB_POST", \
	       'probPrior': "0.10", \
	       'probThres': 0.50, \
	       'maxNumCpt': 1, \
	       'fom': "", \
	       'select01': "", \
	       'select02': "", \
	       'select03': "", \
	       'select04': "", \
	       'select05': "", \
	       'select06': "", \
	       'select07': "", \
	       'select08': "", \
	       'select09': "", \
	       'chatter': 1, \
	       'clobber': "yes", \
	       'debug': "no", \
	       'mode': "ql"}
	
	# Extract catalogue name (mandatory parameter)
	if hasattr(module, 'catname'):
		par['cptCatName'] = get_cat_path() + '/' + module.catname
	else:
		print "ERROR: no 'catname' specified."
	
	# Extract further parameters
	if hasattr(module, 'catid'):              par['cptCatPrefix'] = module.catid
	if hasattr(module, 'density_map'):        par['cptDensFile']  = module.density_map
	if hasattr(module, 'lat_position_error'): par['srcPosError']  = module.lat_position_error
	if hasattr(module, 'position_error'):     par['cptPosError']  = module.position_error
	if hasattr(module, 'prob_method'):        par['probMethod']   = module.prob_method
	if hasattr(module, 'prob_prior'):         par['probPrior']    = module.prob_prior
	if hasattr(module, 'prob_thres'):         par['probThres']    = module.prob_thres
	if hasattr(module, 'max_counterparts'):   par['maxNumCpt']    = module.max_counterparts
	if hasattr(module, 'figure_of_merit'):    par['fom']          = module.figure_of_merit
	if hasattr(module, 'chatter'):            par['chatter']      = module.chatter
	if hasattr(module, 'debug'):              par['debug']        = module.debug
	if hasattr(module, 'new_quantity'):
		list = module.new_quantity
		for index, element in enumerate(list):
			parname      = 'outCatQty0' + str(index+1)
			par[parname] = element
	if hasattr(module, 'selection'):
		list = module.selection
		for index, element in enumerate(list):
			parname      = 'select0' + str(index+1)
			par[parname] = element
		
	# Set result file
	par['outCatName']  = par['cptCatPrefix'].lower() + '.fits'
	
	# Optionally dump parameters
	if chatter:
		print par
	
	# Return parameters
	return par


#==============================#
# Get catalogue from FITS file #
#==============================#
def get_fits_cat(file, catname='GLAST_CAT'):
	"""
	Get catalogue from FITS file.
	
	Arguments:
	 file     catalogue FITS filename
	 catname  catalogue 'EXTNAME'
	Returns:
	 Catalogue HDU
	 If the specified 'catname' has not been found then return
	 the second extension of the FITS file (the first being an empty image).
	"""
	
	# Open FITS files HDU list
	hdulist = pyfits.open(file)
	
	# Search catalogue in HDU list
	for hdu in hdulist:
		cards = hdu.header.ascardlist()
		try:
			if cards['EXTNAME'].value == catname:
				return hdu
		except KeyError:
			pass
	
	# Return second extension
	return hdulist[1]


#==========================#
# Get counterpart name key #
#==========================#
def get_name_key(pars, hdu):
	"""
	Determine counterpart name key from catalogue header.
	
	Arguments:
	 pars  Parameter dictionnary
	 hdu   Header Data Unit of counterpart catalogue
	Returns:
	 Counterpart name key
	 If no name was found returns 'None'
	
	The counterpart name key is first searched using the UCD descriptors
	'ID_MAIN' or 'ID_IDENTIFIER'.
	If no such descriptors were found the following name keys are searched
	for: 'HESS','NAME','ID'.
	"""
	
	# Get HDU cards
	cards = hdu.header.ascardlist()
	
	# Get counterpart prefix
	prefix = col_prefix + pars['cptCatPrefix'] + '_'
	
	# Search for UCD
	for card in cards:
		if card.key[:5] == 'TBUCD' and (card.value == 'ID_MAIN' or \
		   card.value == 'ID_IDENTIFIER'):
			key = 'TTYPE' + card.key[5:7]
			try:
				if cards[key].value.find(prefix) == 0:
					return cards[key].value
			except KeyError:
				pass
	
	# Build possible name string
	names = [prefix + s for s in ['HESS','NAME','ID']]
	
	# Search for possible name strings (case insensitive)
	for card in cards:
		if card.key[:5] == 'TTYPE':
			try:
				if names.index(card.value.upper()) >= 0:
					return card.value
			except ValueError:
				pass
	
	# Return 'None'
	return


#=========================================#
# Attach counterparts to source catalogue #
#=========================================#
def attach_counterparts(pars, hdu_lat):
	"""
	Attach counterparts to LAT catalogue.
	
	Arguments:
	 pars     Parameter dictionnary
	 hdu_lat  HDU of LAT catalogue
	Returns:
	 HDU of LAT catalogue with counterparts attached
	"""
	
	# Stop if LAT catalogue is an ASCII table
	if ('BINTABLE' in hdu_lat.header['XTENSION']):
		pass
	else:
		return hdu_lat

	# Get gtsrcid result catalogue HDU. Stop if catalogue has not been found.
	hdu_cpt = get_fits_cat(pars['outCatName'], catname='GLAST_CAT')
	if hdu_cpt == None:
		return hdu_lat
	
	# Get counterpart name key (None if name key was not found)
	cpt_name_key = get_name_key(pars, hdu_cpt)
	
	# Extract table information
	nrows_lat = hdu_lat.data.shape[0]
	
	# Determine maximum number of counterparts in catalogue. Stop if there are no
	# counterparts
	try:
		max_cpt = max([long(id[9:14]) for id in hdu_cpt.data.field('ID')])
	except AttributeError:
		max_cpt = 0
	if max_cpt < 1:
		return hdu_lat
	
	# Define empty columns list
	columns = []
	
	# Loop over maximum number of counterparts
	for index in range(max_cpt):
		# Build column name strings
		key_name = 'ID_' + pars['cptCatPrefix'] + '_NAME_' + str(index+1)
		key_prob = 'ID_' + pars['cptCatPrefix'] + '_PROB_' + str(index+1)
		
		# Build empty arrays
		array_name = ['' for i in range(nrows_lat)]
		array_prob = [0 for i in range(nrows_lat)]
	
		# Fill arrays
		for row in hdu_cpt.data:
			# Get Array indices
			lat_index = long(row.field('ID')[3:8])-1     # Array index starts with 0
			cpt_index = long(row.field('ID')[9:14])-1    # index starts with 0

			# Use counterpart only if indices correspond
			if cpt_index == index:
				# Get source name
				if cpt_name_key != None:
					name = row.field(cpt_name_key)
				else:
					name = 'NoName'
				
				# Get probability
				prob = row.field('PROB')
				
				# Set array entries
				array_name[lat_index] = name
				array_prob[lat_index] = prob
		
		# Define columns
		column_name = pyfits.Column(name=key_name, format='A20', array=array_name)
		column_prob = pyfits.Column(name=key_prob, format='1E',  array=array_prob, \
		                            unit='probability')
		
		# Append columns
		columns.append(column_name)
		columns.append(column_prob)
	
	# Build new table, append it to LAT catalogue and create new table of combined columns
	hdu_new = pyfits.new_table(columns)
	col_new = hdu_lat.columns + hdu_new.columns
	hdu_new = pyfits.new_table(col_new)
	
	# Copy over LAT catalogue keywords (except basic keywords)
	basic_keys = ['XTENSION', 'BITPIX', 'NAXIS', 'NAXIS1', 'NAXIS2', 'PCOUNT', \
	              'GCOUNT', 'TFIELDS']
	for card in hdu_lat.header.ascardlist():
		if card.key not in basic_keys:
			if card.key.count('TBCOL') or card.key.count('TTYPE') or \
			   card.key.count('TFORM'):
				pass
			else:
				hdu_new.header.update(card.key, card.value, card.comment)
	
	# Return HDU
	return hdu_new


#========================#
# Expand names in string #
#========================#
def expand_string(string, names):
	"""
	Expand names in string.
	
	Arguments:
	 string  String to be expanded
	 names   List of names to be expanded if they occur in string
	"""
	# Initialise pointer to string and get total length of string
	i_string   = 0
	len_string = len(string)

	# Initialise result string
	new_string = ''
	
	# Loop over string
	while i_string < len_string:

		# Initialise search variables
		i_first    = len_string
		replace    = ''
		max_length = 0
		
		# Loop over all names that can be expanded
		for name in names:
			
			# Search for first occurence of name in remaining string
			i_find = string.find(name, i_string)
			
			# Pass if name has not been found
			if i_find == -1:
				pass
			
			# If name has been found earlier than last match, bookkeep name
			elif i_find < i_first:
				i_first    = i_find
				replace    = name
			
			# If name has been found at same position but is longer, bookkeep name
			elif i_find == i_first and len(name) > len(replace):
				replace    = name
		
		# If we found a name to replace then first copy all characters before
		# this name and then attach the expended name to new string
		if len(replace) > 0:
			
			# Get index after name
			i_last = i_first + len(replace)
			
			# Append preceeding characters
			new_string += string[i_string:i_first]
			
			# Append expanded name
			new_string += string[i_first:i_last].replace(replace, '$'+replace+'$', 1)
			
			# Update string pointer
			i_string = i_last
		
		# If no name was found then copy remaining characters and stop
		else:
			new_string += string[i_string:]
			break
		
	# Return expanded string
	return new_string


#================================#
# Get list of valid column names #
#================================#
def expand_column_names(pars):
	"""
	Expand column names in parameter srings by adding $$ signs.
	
	Arguments:
	 pars  Parameter dictronary
	"""
	
	# Set column names list to empty list
	names = []
	
	# Get LAT catalogue names
	hdu = get_fits_cat(pars['srcCatName'], catname='LAT_POINT_SOURCE_CATALOG')
	names.extend([col_prefix+pars['srcCatPrefix']+'_'+s for s in hdu.columns.names])
	
	# Get counterpart catalogue names
	hdu = get_fits_cat(pars['cptCatName'])
	names.extend([col_prefix+pars['cptCatPrefix']+'_'+s for s in hdu.columns.names])
	
	# Expand column names in quantity parameters
	for index in range(1,10):
		parname       = 'outCatQty0' + str(index)
		pars[parname] = expand_string(pars[parname], names)
		
		
	# Expand column names in selection parameters
	for index in range(1,10):
		parname       = 'select0' + str(index)
		pars[parname] = expand_string(pars[parname], names)


#==========================#
# Main routine entry point #
#==========================#
if __name__ == '__main__':
	"""
	Performs source identification for all source classes.
	
	Usage: srcid.py <LATCatalogue> [OPTIONS]
	     -h              Display usage message
	     -C classdir     Specify alternative classes directory
	
	The script runs gtsrid for all source classes that are defined in the
	specified 'classdir'. If no 'classdir' option is given the 'classes'
	directory that is shipped with the distribution is used.
	For each source class a FITS and a log file is created in the current 
	directory, containing the results of the source identification for each
	source class.
	A copy of the LAT input catalogue names 'srcid.fits' is created which
	has the names and counterpart probabilities for all identified sources 
	attached.
	"""
	
	# Verify argument list. We need at least a LAT catalogue name and we allow only
	# for options '-h' and '-C'
	if len(sys.argv) < 2 or sys.argv[1] == '-h' or \
	  (len(sys.argv) > 2 and (len(sys.argv) != 4 or sys.argv[2] != '-C')):
		print 'Usage: srcid.py <LATCatalogue> [OPTIONS]'
		print '     -h              Display this usage message'
		print '     -C classdir     Specify alternative classes directory'
		sys.exit()
	
	# Extract LAT catalogue filename
	lat_filename = sys.argv[1]
	
	# If -C option is specified then extract class directory. Otherwise use classes that
	# ship with this script
	if len(sys.argv) == 4:
		path_classes = sys.argv[3]
		dir_classes  = os.path.basename(path_classes)
		sys.path.insert(0, os.path.dirname(os.path.abspath(path_classes)))  # import classes from here
	else:
		path_script  = os.path.dirname(os.path.abspath(sys.argv[0]))
		dir_classes  = 'classes'
		path_classes = path_script + '/' + dir_classes
	
	# Get list of all source classes, exluding '__init__.py' file
	class_list = glob.glob(path_classes + '/*.py')
	try:
		class_list.remove(path_classes + '/__init__.py')
	except ValueError:
		pass
	
	# Create __init__.py file
	try:
		filename = path_classes + '/__init__.py'
		names    = [os.path.basename(s).rstrip('.py') for s in class_list]
		file     = open(filename, 'w')
		string   = '__all__ = ' + str(names) + '\n'
		file.write('# This file is required so that the current directory is '\
	           'treated as containing packages\n')
		file.write(string)
		file.close()
	except:
		print 'WARNING: Unable to create '+filename+' module'
	
	# Import source classes now
	class_import_string = 'from ' + dir_classes + ' import *'
	exec(class_import_string)
	
	# Get LAT source catalogue HDU
	hdu_lat = get_fits_cat(lat_filename, catname='LAT_POINT_SOURCE_CATALOG')
	
	# Loop over all source classes
	for class_one in class_list:
		
		# Get source class module name
		class_name = os.path.basename(class_one).rstrip('.py')
		this       = eval(class_name)
		
		# Get source class parameters
		pars = set_pars(this, chatter=False)
		
		# Set LAT catalogue filename
		pars['srcCatName'] = lat_filename
		
		# Expand column names in parameter strings
		expand_column_names(pars)
		
		# Set gtsrcid command
		cmd = set_command("gtsrcid", pars)
		
		# Run gtsrcid
		error, result = run_command(cmd)
		#run_gtsrcid(pars)
		
		# Rename result file
		os.rename('gtsrcid.log', pars['cptCatPrefix'].lower() + '.log')
		
		# Attach counterparts to LAT catalogue
		hdu_lat = attach_counterparts(pars, hdu_lat)
		
	# Save LAT catalogue with attached columns
	hdu_lat.writeto('srcid.fits', clobber=True)
