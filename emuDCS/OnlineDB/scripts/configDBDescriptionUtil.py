#!/bin/env python

# Run with -h option to see documentation

from optparse import OptionParser, OptionGroup
import cx_Oracle
import os
import sys
import re
import subprocess
from datetime import datetime
from dateutil import parser as date_parser # will use that instead of datetime.strptime that is only python>=2,5

connection_string = ''

DBG = False


def script_parameters():
   
  usage="""Usage: %prog [options] [file.txt]

  This script is either for listing or for updating the descriptions of 
  CSC Peripheral Crates configuration data entries in DB.

*** DB connection

  By default, it connects to cms_rcms, online DB.
  You may override it with --db=yourdbname option.
  
  If you have a ./dbuserfile.txt or ~/dbuserfile.txt, its contents 
  would be used for authentication.
  Otherwise, you would be prompted for username and password.

*** HOW TO LIST CONFIG IDs AND THEIR DESCRIPTIONS
  
  Some examples of listing (option -l or --list) with extra selection options:

./%prog -l --side minus --min_id 2000130
./%prog -l --side plus --min_date '2012-1-1 21:00:00'

  This would read information from DB with given selections and print it to screen.
  If you would provide an optional file name argument, e.g.,

./%prog -l --side minus --min_id 2000130 ids_after2000130.txt

  the listing result would be stored in this file.
  The file would have format suitable for further use with --update.

*** HOW TO UPDATE DESCRIPTIONS IN DB

  * WARNING *:
  Always back up the descriptions that are currently in DB before doing any update, e.g.:
  
./%prog -l backup_EmuCfgDescr__`date +%Y_%m_%d-%H_%M_%S`.txt
  
  Performing description update is invoked with -u or --update option:

./%prog -u ids_after2000130.txt

  The input file name is required. This file should contain one or 
  several ID description entries separated by empty lines where 
  each ID description entry is expected to have the following format:

2000022
# any lines starting with # are ignored by script as comments
short description for this ID should be provided here (single line 6-50 characters long)
optional long description for this ID could be provided here (single line of < 949 characters)

  Note that if you would provide no description at all for an ID, this ID entry
  would not be parsed. If any description length would be out of bounds, script
  would exit and advise you to fix those descriptions in the file.
  
  The most convenient way to create such a file for further editing 
  is to run --list command with a file name script argument.
  
  An important option is -n or --dry_run which allows you to run the update procedure
  without actually updating the DB.
--------------------------------------------------------------------------------"""

  parser = OptionParser(usage = usage)

  g0 = OptionGroup(parser,"Connection")
  g0.add_option('--db',
    help='name of the database to connect. The default value is "cms_rcms" '+
         '(or "XE" if domain name is physics.tamu.edu)',
    type="string",
    default='cms_rcms',
    dest="dbname")

  g1 = OptionGroup(parser,"Update options")
  g1.add_option('-u', '--update',
    help='Update the descriptions for IDs provided in a specially formatted text file '+
         'with its filename provided as a single argument of the script.',
    action="store_true",
    dest="update")
  g1.add_option('--no_prompt',
    help='if this option is present, no (y/n) confirmation prompt would be offered before updating the DB. '+
         'If the option is not present, user would be asked to confirm the description change in DB for each ID.',
    action="store_true",
    dest="no_prompt")
  g1.add_option('-n','--dry_run',
    help='Do everything except writing to DB.',
    action="store_true",
    dest="dry_run")

  g2 = OptionGroup(parser,"Listing options")
  g2.add_option('-l', '--list',
    help='list current descriptions. The following options allow to control the selection: '+
         '  --minid, --maxid, --min_date, --max_date, --side. ' + 
	 'If file.txt argument is provided, the result is written to a text file in a format '+
	 'that is suitable for further modification and use with the --update option',
    action="store_true",
    dest="ls")
  g2.add_option('--min_id',
    help='minimum ID to select',
    type="string",
    default='0',
    dest="min_id")
  g2.add_option('--max_id',
    help='maximum ID to select',
    type="string",
    default='100000000000',
    dest="max_id")
  g2.add_option('--min_date',
    help='lower date for a date range selection. Recommended format is "2011-1-22 15:00:00"',
    type="string",
    default='2000-1-1',
    dest="min_date")
  g2.add_option('--max_date',
    help='upper date for a date range selection. Recommended format is "2011-1-22 15:00:00"',
    type="string",
    default='2200-1-1',
    dest="max_date")
  g2.add_option('-s', '--side',
    help='endcap side for selection with --list. If provided, must have value either "plus" or "minus"',
    type="string",
    default='',
    dest="side")

  parser.add_option_group(g0)
  parser.add_option_group(g2)
  parser.add_option_group(g1)

  opts, args = parser.parse_args()

  script_help = sys.argv[0] + ' -h\n'
  
  if len(sys.argv) == 1:
    parser.print_help()
    sys.exit(0)

  if opts.ls and opts.update:
    print '\nOnly one of the --list or --update options must be given!\n'
    sys.exit(0)

  # a custom class in order not to be dependent on and to extend what we get from OptionParser
  class Cfg:
    def __init__(self):
      self.update = False
      self.ls = True
      self.min_id = 0
      self.max_id = 10000000000
      self.side = None
      self.min_date = datetime(2000, 1, 1, 0, 0, 0, 0)
      self.max_date = datetime(2200, 1, 1, 0, 0, 0, 0)
      self.file = ''
      self.prompt = True
      self.dry_run = False
      self.dbname = 'cms_rcms'

  cfg = Cfg()

  if opts.update:
    cfg.update = True
    cfg.ls = False
    if len(args) < 1:
      print '\nFilename must be provided as an argument for the update operation! See'
      print script_help
      sys.exit(0)
    cfg.file = args[0]
    if opts.no_prompt:  cfg.prompt = False
    if opts.dry_run:    cfg.dry_run = True
  
  if opts.ls:
    cfg.ls = True
    cfg.update = False
    cfg.min_id = opts.min_id
    cfg.max_id = opts.max_id
    
    try:
      #cfg.min_date = datetime.strptime(opts.min_date, "%Y_%m_%d")
      #cfg.max_date = datetime.strptime(opts.max_date, "%Y_%m_%d")
      cfg.min_date = date_parser.parse(opts.min_date)
      cfg.max_date = date_parser.parse(opts.max_date)
    except ValueError:
      print '\nDates are not in proper format! See'
      print script_help
      raise
      
    if opts.side != '':
      if opts.side == 'plus' or opts.side == 'minus':
        cfg.side = opts.side
      else:
        print '\n--side must have value either "plus" or "minus"! See'
        print script_help
        sys.exit(0)
    if len(args) > 0:
      cfg.file = args[0]

  if DBG: print cfg
  return cfg



def connection_setup(dbname):

  # if there is non-empty global connection_string, honour it:
  global connection_string
  if connection_string != '': return

  # if running at TAMU, change the default to XE (should be defined in tnsnames.ora)
  if dbname == 'cms_rcms':
    process = subprocess.Popen(['hostname', '-d'], shell=False, stdout=subprocess.PIPE)
    domain = process.communicate()[0].strip()
    if domain == 'physics.tamu.edu':
      dbname = 'XE'
  
  # check if ./dbuserfile.txt or ~/dbuserfile.txt is present
  dbuserfile = None
  if os.path.isfile("dbuserfile.txt"):
    dbuserfile = 'dbuserfile.txt'
  elif os.path.isfile(os.environ['HOME'] + '/dbuserfile.txt'):
    dbuserfile = os.environ['HOME'] + '/dbuserfile.txt'
  
  if  dbuserfile is not None:
    f = open(dbuserfile, 'r')
    auth_info = f.read(1024).strip()
    f.close()
    
  # if not, prompt for the authentication:
  else:
    while True:
      uname = raw_input('Username: ').strip()
      passwd = raw_input('Password: ').strip()
      print
      if uname == '' or uname is None:
        print "You entered empty username!"
        continue
      break
    auth_info = uname + '/' + passwd

  connection_string = auth_info + '@' + dbname



def encode_description_string(desc_short, desc_long):
  if desc_long == '':
    desc = desc_short
  else:
    desc = desc_short + '\n' + desc_long
  if len(desc) > 1000:
    print 'WARNING: full description was truncated to 1000 characters, was', len(desc)
    desc = desc[:1000]
  return desc



def parse_description_string(desc):
  desc = desc.strip()
  newline = desc.find('\n')
  if newline == -1:
    return (desc, '')
  return (desc[:newline], desc[newline+1:])

  

def read_descriptions_from_db(cfg):

  query = """SELECT EMU_CONFIG_ID, EMU_CONFIG_TIME, DESCRIPTION  FROM EMU_CONFIGURATION
             WHERE EMU_CONFIG_ID BETWEEN :min_id AND :max_id 
             AND EMU_CONFIG_TIME BETWEEN :min_date AND :max_date"""

  params = {
    'min_id': cfg.min_id,
    'max_id': cfg.max_id,
    'min_date': cfg.min_date,
    'max_date': cfg.max_date,
  }

  if cfg.side is not None: 
    query += " AND EMU_ENDCAP_SIDE = :side"
    params['side'] = cfg.side

  db = cx_Oracle.connect(connection_string)
  cursor = db.cursor()
  cursor.prepare(query)
  cursor.execute(None, params)

  data = {}
  for row in cursor:
    if DBG: print row
    desc_short, desc_long = parse_description_string(row[2])
    data[str(row[0])] = {
      'desc_short': desc_short,
      'desc_long': desc_long,
      'date' : row[1]
    }

  cursor.close()
  db.close()
  return data



def print_data(data):
  ids = data.keys()
  ids.sort()
  for id in ids:
    print "%s : %s : %s" % (id, data[id]['date'].strftime("%Y-%m-%d %H:%M:%S") , data[id]['desc_short'])
    #print "  ",data[id]['desc_short']
    if data[id]['desc_long'] != '':
      print "  ",data[id]['desc_long']
    print
  print "Number of IDs selected:", len(ids)



def write_data_to_file(fname, data):
  ids = data.keys()
  ids.sort()
  f = open(fname, 'w')
  try:
    for id in ids:
      f.write(id + '\n')
      f.write('# ' + data[id]['date'].strftime("%Y-%m-%d %H:%M:%S") + '\n')
      f.write(data[id]['desc_short'] + '\n')
      if data[id]['desc_long'] != '':
        f.write(data[id]['desc_long'] + '\n')
      f.write('\n')
  finally:
    f.close()



def read_data_from_file(fname):

  f = open(fname, 'r')
  try:
    # read everything, but no more then 5MB
    text = f.read(1024*1024*5)
    if len(text) >= 1024*1024*5:
      print 'You are trying to read file that is larger then 5 MB. Something is definitely wrong! Exiting...'
      sys.exit(0)
  finally:
    f.close()
    
  # replace '\n\n' -> '\n\n\n\n' for easier regexp matching without overlaps later
  text = re.sub(r'\n\n', r'\n\n\n\n', text)
  
  # add two newline to the front:
  text = '\n\n' + text
  
  # make sure there is '\n\n' in the end
  text += '\n\n'
  
  # collapse multiple spaces in the beginnings of lines
  text = re.sub(r'\n +', r'\n', text)

  # remove comment lines
  text = re.sub(r'\n\#.*\n', r'\n', text)
  
  # match to the ID description pattern
  descs = re.findall(r'\n\n(\d+)\n(.+\n?.+)\n\n' , text)
  
  data = {}
  bad_descriptions = []
  for d in descs:
    # remove any possible initial or trailing spaces or newlines from the description:
    desc = d[1].strip()
    # split description in short and (optional) long parts
    desc_short, desc_long = parse_description_string(desc)
    
    if len(desc_short) <= 5: 
      bad_descriptions.append(d[0] + " short description's length should be >5 characters. Current len=%d" % len(desc_short))
    if len(desc_short) > 50: 
      bad_descriptions.append(d[0] + " short description's length should be <=50 characters. Current len=%d" % len(desc_short))
    if len(desc_long) > 949: 
      bad_descriptions.append(d[0] + " long description's length should be <=949 characters. Current len=%d" % len(long_short))
    
    data[d[0]] = {
      'desc_short': desc_short,
      'desc_long': desc_long,
      'date' : datetime(2000,1,1) # dummy date
    }
  
  if len(bad_descriptions) > 0:
    print "\nDescription length issues found:"
    for ln in bad_descriptions:
      print "  ", ln
    print "\nThis has to be fixed in the file in order to proceed with update!\n"
    data = {}

  return data



def db_read_desc_for_id(id):

  db = cx_Oracle.connect(connection_string)
  cursor = db.cursor()
  cursor.execute('SELECT EMU_CONFIG_ID, EMU_CONFIG_TIME, DESCRIPTION FROM EMU_CONFIGURATION WHERE EMU_CONFIG_ID = ' + id)
  row = cursor.fetchone()
  cursor.close()
  db.close()

  if row is None:
    print 'Failed to read configuration description for ID ' + id + ' (no such ID?)'
    return None

  desc_short, desc_long = parse_description_string(row[2])
  data = {
    'desc_short': desc_short,
    'desc_long': desc_long,
    'date' : row[1]
  }
  return data



def update_db_description_for_id(id, desc):

  # SQL'ize any possible single quote marks ' -> ''
  desc = desc.replace("'","''")

  # remove any possible initial or trailing spaces or newlines:
  desc = desc.strip()
  
  db = cx_Oracle.connect(connection_string)
  cursor = db.cursor()
  cursor.execute("UPDATE EMU_CONFIGURATION SET DESCRIPTION='" + desc + "' WHERE EMU_CONFIG_ID = " + id)
  db.commit()
  print " --> description was updated in DB"
  cursor.close()
  db.close()



def update_db_descriptions(data, prompt_user=True, dry_run=False):

  ids = data.keys()
  ids.sort()
  n = 0
  for id in ids:
    n += 1
    # fetch current description:
    indb = db_read_desc_for_id(id)
    if indb is None:
      continue
    print '\nID %s : %s   (%d/%d):' % (id, indb['date'].strftime("%Y-%m-%d %H:%M:%S"), n, len(data))
    print 'Do you want to replace the current description'
    print '  Short: ' + indb['desc_short']
    print '   Long: ' + indb['desc_long']
    print 'with new description'
    print '  Short: ' + data[id]['desc_short']
    print '   Long: ' + data[id]['desc_long']

    # ask user for confirmation if --no_prompt option was not provided
    do_update = True
    while prompt_user:
      ans = raw_input('Confirm to write to DB (y/n): ')
      if ans not in ['y', 'Y', 'n', 'N']:
        print 'please enter y or n.'
        continue
      if ans == 'y' or ans == 'Y':
        break
      if ans == 'n' or ans == 'N':
        do_update = False
        break
    
    if do_update:
      full_desc = encode_description_string(data[id]['desc_short'], data[id]['desc_long'])
      if not dry_run:
        update_db_description_for_id(id, full_desc)



if __name__ == '__main__':
  
  cfg = script_parameters()
  
  connection_setup(cfg.dbname)
  
  if cfg.ls:
    
    data = read_descriptions_from_db(cfg)
    
    if cfg.file == '':
      print_data(data)
    
    else:
      write_data_to_file(cfg.file, data)
      print "Number of IDs selected:", len(data)
      print "Written to a file ", cfg.file
    
  elif cfg.update:
    
    data = read_data_from_file(cfg.file)
    if DBG: print_data(data)
    
    if len(data) > 0:
      print "\nSuccesfully read", len(data), "ID descriptions from", cfg.file
    else:
      print "No ID descriptions to update."
    
    update_db_descriptions(data, cfg.prompt, cfg.dry_run)
