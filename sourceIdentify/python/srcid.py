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
import numpy                # Numerical arrays
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
	#print error
	#print result
	
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
	# Initialise found flag
	found = False
	
	# First check for GLAST_CAT
	try:
		path = os.environ['GLAST_CAT']
		if os.path.isdir(path):
			found = True
			#print 'Found GLAST_CAT: ',path
	except:
		pass
	
	# Then check for EXTFILESSYS
	if not found:
		try:
			path = os.path.join(os.environ['EXTFILESSYS'],'catalogues')
			if os.path.isdir(path):
				found = True
				#print 'Found EXTFILESSYS: ',path
		except:
			pass
	
	# Then check of GLAST_EXT
	if not found:
		try:
			pattern = os.environ['GLAST_EXT']+'/extFiles/*'
			dirs    = glob.glob(pattern)
			dirs.sort(reverse=True)
			if len(dirs) > 0:
				path = dirs[0]+'/counterpartCatalogs'
				if os.path.isdir(path):
					found = True
					#print 'Found GLAST_EXT: ',path
		except:
			pass
	
	# Signal error in no path has been found
	if not found:
		print 'ERROR: Catalogue repository cound not be localized.'
		print '       Set the environment variable GLAST_CAT to the directory where'
		print '       all catalogues reside or set GLAST_EXT to the external'
		print '       ScienceTools libraries.'
		sys.exit()
	
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
		sys.exit(0)
	
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
	
	# If we have not found the requested extension then return
	# the second in the list
	if len(hdulist) >= 2:
		return hdulist[1]
	else:
		print 'ERROR: Catalogue extension not found in file "'+str(file)+'"'
		sys.exit(0)


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
	for: 'HESS','NAME','ID','NICKNAME'.
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
	names = [prefix + s for s in ['HESS','NAME','ID', 'NICKNAME']]
	
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
		key_ra   = 'ID_' + pars['cptCatPrefix'] + '_RA___' + str(index+1)
		key_dec  = 'ID_' + pars['cptCatPrefix'] + '_DEC__' + str(index+1)
		key_sep  = 'ID_' + pars['cptCatPrefix'] + '_ASEP_' + str(index+1)
		
		# Build empty arrays
		array_name = ['' for i in range(nrows_lat)]
		array_prob = [0 for i in range(nrows_lat)]
		array_ra   = [0.0 for i in range(nrows_lat)]
		array_dec  = [0.0 for i in range(nrows_lat)]
		array_sep  = [0.0 for i in range(nrows_lat)]
		
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
				ra   = row.field('RAJ2000')
				dec  = row.field('DEJ2000')
				sep  = row.field('ANGSEP')
				
				# Set array entries
				array_name[lat_index] = name
				array_prob[lat_index] = prob
				array_ra[lat_index]   = ra
				array_dec[lat_index]  = dec
				array_sep[lat_index]  = sep
		
		# Define columns
		column_name = pyfits.Column(name=key_name, format='A20', array=array_name)
		column_prob = pyfits.Column(name=key_prob, format='1E',  array=array_prob, \
		                            unit='probability')
		column_ra   = pyfits.Column(name=key_ra,   format='1E',  array=array_ra, unit='deg')
		column_dec  = pyfits.Column(name=key_dec,  format='1E',  array=array_dec, unit='deg')
		column_sep  = pyfits.Column(name=key_sep,  format='1E',  array=array_sep, unit='deg')
		
		# Append columns
		columns.append(column_name)
		columns.append(column_prob)
		columns.append(column_ra)
		columns.append(column_dec)
		columns.append(column_sep)
	
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
	
	# Expand column names in method, Prior, and FOM
	pars['probMethod'] = expand_string(str(pars['probMethod']), names)
	pars['probPrior']  = expand_string(str(pars['probPrior']), names)
	pars['fom']        = expand_string(str(pars['fom']), names)
	
	# Expand column names in quantity parameters
	for index in range(1,10):
		parname       = 'outCatQty0' + str(index)
		pars[parname] = expand_string(pars[parname], names)
		
		
	# Expand column names in selection parameters
	for index in range(1,10):
		parname       = 'select0' + str(index)
		pars[parname] = expand_string(pars[parname], names)


#========================================#
# Create LAT format compatible catalogue #
#========================================#
def create_lat_cat(lat_name, srcid_name, out_name, cpt_cats):
	"""
	Create catalogue that is compatible with the LAT catalogue format.
	
	Parameters:
	 lat_name:   Name of the original LAT catalogue
	 srcid_name: Name of the original srcid.py catalogue
	 out_name:   Name of the output catalogue
	 cpt_cats:   List of counterpart catalogues
	"""
	# Get LAT source catalogue HDU
	hdu_lat = get_fits_cat(lat_name, catname='LAT_POINT_SOURCE_CATALOG')
	nrows   = hdu_lat.data.shape[0]
	
	# Get srcid result catalogue
	hdu_srcid = get_fits_cat(srcid_name, catname='LAT_POINT_SOURCE_CATALOG')
	
	# Build empty ID list
	idlist = [[] for row in range(nrows)]
	
	# Build identification list
	max_cpt = 0
	for column in hdu_srcid.columns.names:
		if (column.find('ID_') != -1 and column.find('_NAME_') != -1):
			
			# Set column names to read
			col_name = column
			col_prob = column.replace('_NAME_', '_PROB_')
			col_ra   = column.replace('_NAME_', '_RA___')
			col_dec  = column.replace('_NAME_', '_DEC__')
			col_sep  = column.replace('_NAME_', '_ASEP_')
			
			# Read columns
			names = hdu_srcid.data.field(col_name)
			probs = hdu_srcid.data.field(col_prob)
			ra    = hdu_srcid.data.field(col_ra)
			dec   = hdu_srcid.data.field(col_dec)
			sep   = hdu_srcid.data.field(col_sep)
			
			# Loop over all rows and extract information
			for i, name in enumerate(names):
				if (name != ''):
					# Search catalogue number
					catid = 0
					for cat in cpt_cats:
						pattern = '_'+cat['label']+'_'
						if (col_name.find(pattern) != -1):
							catid = cat['number']
					# Build entry
					entry = {'name': name, 'prob': probs[i], 'ra': ra[i], \
					         'dec': dec[i], 'angsep': sep[i], 'cat': catid}
					idlist[i].append(entry)
					if len(idlist[i]) > max_cpt:
						max_cpt = len(idlist[i])
	
	# Stop if there were no counterparts
	if max_cpt == 0:
		print 'WARNING: No counterparts found, no LAT format compatible catalogue built.'
		return

	
	# Set column format strings
	num_cpt  = max_cpt
	fmt_name = '%d' % (num_cpt*20) + 'A20'
	fmt_prob = '%d' % num_cpt + 'E'
	fmt_cat  = '%d' % num_cpt + 'I'
	
	# Define new table columns
	column_number = pyfits.Column(name='ID_Number', format='I')
	column_name   = pyfits.Column(name='ID_Name', format=fmt_name)
	column_prob   = pyfits.Column(name='ID_Probability', format=fmt_prob)
	column_ra     = pyfits.Column(name='ID_RA', format=fmt_prob)
	column_dec    = pyfits.Column(name='ID_DEC', format=fmt_prob)
	column_sep    = pyfits.Column(name='ID_Angsep', format=fmt_prob)
	column_cat    = pyfits.Column(name='ID_Catalog', format=fmt_cat)
	
	# Collect all columns
	columns = [column_number, column_name, column_prob, column_ra, column_dec, \
	           column_sep, column_cat]
	
	# Build new table, append it to LAT catalogue and create new table of combined columns
	hdu_new = pyfits.new_table(columns)
	col_new = hdu_lat.columns + hdu_new.columns
	hdu_new = pyfits.new_table(col_new)
	
	# Define catalogue table columns
	ncats       = len(cpt_cats)
	array       = numpy.zeros(ncats)
	column_cat  = pyfits.Column(name='ID_Catalog', format='I', array=array)
	column_name = pyfits.Column(name='Name', format='A50')
	column_ref  = pyfits.Column(name='Reference', format='A255')
	column_url  = pyfits.Column(name='URL', format='A255')
	
	# Collect columns
	columns = [column_cat, column_name, column_ref, column_url]
	
	# Build new table
	hdu_cat = pyfits.new_table(columns)
	
	# Fill new columns
	data_number = hdu_new.data.field('ID_Number')
	data_name   = hdu_new.data.field('ID_Name')
	data_prob   = hdu_new.data.field('ID_Probability')
	data_ra     = hdu_new.data.field('ID_RA')
	data_dec    = hdu_new.data.field('ID_DEC')
	data_sep    = hdu_new.data.field('ID_Angsep')
	data_cat    = hdu_new.data.field('ID_Catalog')
	for i, row in enumerate(idlist):
		
		# Determine the number of counterparts
		nids           = len(row)
		data_number[i] = nids
		
		# Sort list
		row.sort(compare_by('prob'), reverse=True)
		
		# Loop over all entries
		names = ''
		for k, cpt in enumerate(row):
			name = cpt['name']
			while (len(name) < 20):
				name = name + ' '
			names = names + name
#			if nids > 1:
			if num_cpt > 1:
				data_prob[i][k] = cpt['prob']
				data_ra[i][k]   = cpt['ra']
				data_dec[i][k]  = cpt['dec']
				data_sep[i][k]  = cpt['angsep']
				data_cat[i][k]  = cpt['cat']
			else:
				data_prob[i] = cpt['prob']
				data_ra[i]   = cpt['ra']
				data_dec[i]  = cpt['dec']
				data_sep[i]  = cpt['angsep']
				data_cat[i]  = cpt['cat']
		data_name[i] = names
	
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
	
	# Reformat the 'ID_Name' column
	cards = hdu_new.header.ascardlist()
	for card in cards:
		if card.value == 'ID_Name':
			key   = card.key.replace('TTYPE','TFORM')
			value = fmt_name
			hdu_new.header.update(key, value)
	
	# Fill catalogue table
	data_cat  = hdu_cat.data.field('ID_Catalog')
	data_name = hdu_cat.data.field('Name')
	data_ref  = hdu_cat.data.field('Reference')
	data_url  = hdu_cat.data.field('URL')
	for i, cat in enumerate(cpt_cats):
		data_cat[i]  = cat['number']
		data_name[i] = cat['name']
		data_ref[i]  = cat['ref']
		data_url[i]  = cat['url']
	
	# Add keywords to catalogue table
	hdu_cat.header.update('EXTNAME', 'ID_CAT_REFERENCE')
	
	# Build result catalogue HDU list. If additional HDUs were present in the input
	# catalogue then add them at the end ...
	hdu_list = [pyfits.PrimaryHDU(), hdu_new, hdu_cat]
	latcat   = pyfits.open(lat_name)
	if len(latcat) > 2:
		for hdu in latcat[2:]:
			hdu_list.append(hdu)
	
	# Save LAT catalogue with attached columns
	hdulist = pyfits.HDUList(hdu_list)
	hdulist.writeto(out_name, clobber=True)
	latcat.close()
	
	# Get Source information column names
	cards   = hdu_lat.header.ascardlist()
	colname = 'none'
	colra   = 'none'
	coldec  = 'none'
	colmaj  = 'none'
	colmin  = 'none'
	colpos  = 'none'
	for card in cards:
		
		# Search for Name column
		if card.key[:5] == 'TBUCD' and (card.value == 'ID_MAIN' or \
		   card.value == 'ID_IDENTIFIER'):
			key     = 'TTYPE' + card.key[5:7]
			colname = cards[key].value
		elif (str(card.value).upper() == 'ID' or str(card.value).upper() == 'NAME' or \
		      str(card.value).upper() == 'NICKNAME'):
			key     = 'TTYPE' + card.key[5:7]
			colname = cards[key].value
		elif (str(card.value).upper() == 'RA'):
			key   = 'TTYPE' + card.key[5:7]
			colra = cards[key].value
		elif (str(card.value).upper() == 'DEC'):
			key    = 'TTYPE' + card.key[5:7]
			coldec = cards[key].value
		elif (str(card.value).upper() == 'POSERR95'):
			key    = 'TTYPE' + card.key[5:7]
			colmaj = cards[key].value
		elif (str(card.value).upper() == 'CONF_95_SEMIMAJOR'):
			key    = 'TTYPE' + card.key[5:7]
			colmaj = cards[key].value
		elif (str(card.value).upper() == 'CONF_95_SEMIMINOR'):
			key    = 'TTYPE' + card.key[5:7]
			colmin = cards[key].value
		elif (str(card.value).upper() == 'CONF_95_POSANG'):
			key    = 'TTYPE' + card.key[5:7]
			colpos = cards[key].value
	
	# Create region file
	regfile = open("srcid.reg", "w")
	
	# Write header
	regfile.write('# Region file format: DS9 version 4.0\n')
	regfile.write('# Created by srcid.py\n')
	regfile.write('# Filename: srcid-lat.fits\n')
	regfile.write('global ')
	regfile.write('color=blue ')
	regfile.write('point=diamond ')
	regfile.write('font="helvetica 9 normal" ')
	regfile.write('select=1 ')
	regfile.write('highlite=1 ')
	regfile.write('edit=1 ')
	regfile.write('move=0 ')
	regfile.write('delete=1 ')
	regfile.write('include=1 ')
	regfile.write('fixed=0 ')
	regfile.write('source ')
	regfile.write('\n')
	regfile.write('fk5\n')
	
	# Write entry for each LAT source
	for i, row in enumerate(idlist):
		
		# Initialise empty region
		lat_region = ''
		
		# Get LAT source information
		lat_name = 'unknown'
		if (colname != 'none'):
			lat_name = hdu_lat.data.field(colname)[i]
		if (colra != 'none' and coldec != 'none'):
			ra  = '%8.4f' % hdu_lat.data.field(colra)[i]
			dec = '%8.4f' % hdu_lat.data.field(coldec)[i]
			if (colmaj != 'none'):
				if (colmin != 'none' and colpos != 'none'):
					vmaj = hdu_lat.data.field(colmaj)[i]
					vmin = hdu_lat.data.field(colmin)[i]
					vang = hdu_lat.data.field(colpos)[i] + 90.0
					if vang >= 360.0:
						vang = vang - 360.0
					if abs(vmaj) == float("inf"):
						vmaj = 1.0 / 60.0
					if abs(vmin) == float("inf"):
						vmin = 1.0 / 60.0
					if str(vmaj) == "nan":
						vmaj = 1.0 / 60.0
					if str(vmin) == "nan":
						vmin = 1.0 / 60.0
					smaj = '%8.5f' % vmaj
					smin = '%8.5f' % vmin
					pang = '%8.5f' % vang
					lat_region = 'ellipse('+ra+','+dec+','+ \
					              smaj+','+smin+','+pang+')'
				else:
					rad = '%8.5f' % hdu_lat.data.field(colmaj)[i]
					lat_region = 'circle('+ra+','+dec+','+rad+')'
		
		# Write LAT source header
		regfile.write('#\n')
		regfile.write('# '+lat_name+'\n')
		
		# Determine the number of counterparts
		nids           = len(row)
		data_number[i] = nids
		
		# Sort list
		row.sort(compare_by('prob'), reverse=True)
		
		# Loop over all counterparts
		for k, cpt in enumerate(row):
			
			# Gather counterpart information
			cpt_name = cpt['name']
			cpt_ra   = '%8.4f' % cpt['ra']
			cpt_dec  = '%8.4f' % cpt['dec']
			
			# Write entry
			regfile.write('point('+cpt_ra+','+cpt_dec+') ')
			regfile.write('# ')
			regfile.write('text={'+cpt_name+'}\n')
		
		# Write LAT entry
		regfile.write(lat_region)
		regfile.write(' #')
		regfile.write(' color=green')
		regfile.write(' font="helvetica 11 normal"')
		regfile.write(' text={'+lat_name+'}\n')

	# Close region file
	regfile.close()


#================================#
# Dictionary comparison function #
#================================#
def compare_by(key):
	def compare2dicts(a,b):
		return cmp(a[key],b[key])
	return compare2dicts


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
	class_list.sort()
	
	# Create __init__.py file
	try:
		filename = path_classes + '/__init__.py'
		names    = [os.path.basename(s).rstrip('py').rstrip('.') for s in class_list]
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
	try:
		hdu_lat = get_fits_cat(lat_filename, catname='LAT_POINT_SOURCE_CATALOG')
	except:
		print 'ERROR: Catalogue ' + lat_filename + ' not found.'
		sys.exit(0)
	
	# Initialise counterpart catalogue dictionary list
	cpt_cats  = []
	cpt_index = 1
	
	# Loop over all source classes
	for class_one in class_list:
		
		# Get source class module name
		class_name = os.path.basename(class_one).rstrip('py').rstrip('.')
		this       = eval(class_name)
		
		# Get source class parameters
		pars = set_pars(this, chatter=False)
		
		# Set LAT catalogue filename
		pars['srcCatName'] = lat_filename
		
		# Check if counterpart catalogue exists
		cpt_url = pars['cptCatName']
		if not os.path.isfile(cpt_url):
			print "WARNING: Catalogue '"+cpt_url+"' not found in "+ \
			      "catalogue repository. Skipped."
			continue
		
		# Expand column names in parameter strings
		expand_column_names(pars)
		
		# Get counterpart catalogue information
		try:
			# Read counterpart catalogue
			hdu_cpt = get_fits_cat(cpt_url)
			
			# Initialise information fields
			cpt_label  = pars['cptCatPrefix']
			cpt_number = cpt_index
			cpt_name   = ''
			cpt_ref    = ''
			
			# Gather catalogue name and reference from catalogue
			cards      = hdu_cpt.header.ascardlist()
			for card in cards:
				# Gather catalogue name
				if card.key == 'CAT-NAME':
					cpt_name = hdu_cpt.header['CAT-NAME']
				if card.key == 'EXTNAME' and cpt_name == '':
					cpt_name = hdu_cpt.header['EXTNAME']
				if card.key == 'CDS-NAME' and cpt_name == '':
					cpt_name = hdu_cpt.header['CDS-NAME']
				
				# Gather catalogue reference
				if card.key == 'CAT-REF':
					cpt_ref = hdu_cpt.header['CAT-REF']
				if card.key == 'AUTHOR' and cpt_ref == '':
					cpt_ref = hdu_cpt.header['AUTHOR']
				if card.key == 'CDS-CAT' and cpt_ref == '':
					cpt_ref = hdu_cpt.header['CDS-CAT']
			
			# Set catalogue information
			info = {'number': cpt_number, 'label': cpt_label, 'name': cpt_name, \
			        'ref': cpt_ref, 'url': 'file:/' + cpt_url}
			cpt_cats.append(info)
			
		except:
			print 'WARNING: Catalogue ' + cpt_url + 'not found.'
			continue
		
		# Dump processing information to screen
		print 'Process ' + info['name'] + ' (' + info['url'] + ')'
		
		# Set gtsrcid command
		cmd = set_command("gtsrcid", pars)
		
		# Run gtsrcid
		error, result = run_command(cmd)
		if error != 0:
			print 'WARNING: gtsrcid error while processing catalogue ' + cpt_url
			lines = result.splitlines(False)
			for line in lines:
				print '         '+line
			continue
		
		# Rename result file
		os.rename('gtsrcid.log', pars['cptCatPrefix'].lower() + '.log')
		
		# Attach counterparts to LAT catalogue
		hdu_lat = attach_counterparts(pars, hdu_lat)
		
		# Increment index
		cpt_index = cpt_index + 1
		
	# Save LAT catalogue with attached columns
	hdu_lat.writeto('srcid.fits', clobber=True)

	# Create LAT compatible catalogue format
	create_lat_cat(lat_filename, 'srcid.fits', 'srcid-lat.fits', cpt_cats)
