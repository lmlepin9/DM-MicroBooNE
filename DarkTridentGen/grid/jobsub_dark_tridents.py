#!/usr/bin/env python    

##################################################                                                                                                                                                                                                                              
# Dark Tridents grid job submitter                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
# Made to work with parameter_uboone_template_grid.dat                                                                                                                                                                                                                                 
# 2020.01 -- A.Navrer-Agasson                                                                                                                                                                                                                                                   
##################################################                                                                                                                                                                                                                              
import os, optparse, random, shutil, tarfile, sys
import subprocess, string
import time


PWD = os.getenv("PWD")

##################################################                                                                                                                                                                                                                              
# Job Defaults                                                                                                                                                                                                                                                                  
################################################## 
USER                  = os.getenv("USER") 
DO_TAR                = True
MA_DIR                = 0.05 
RATIO                 = 0.6                                                                                                                                                                                                                           
NEVTS                 = 50
N_JOBS                = 100
ALD                   = 0.1
DM_TYPE               = "scalar"
DECAY_CHAN            = "all"
RUN_NUMBER            = 20220727
SIGNAL                = "parameter_space"
OUTDIR                = "/pnfs/uboone/scratch/users/{USER}/pawel_dm_gen/{DECAY_CHAN}/run{RUN_NUMBER}/{mA}/files/".format( USER = os.getenv("USER"), DECAY_CHAN = DECAY_CHAN, mA = MA_DIR, RUN_NUMBER = RUN_NUMBER)
LOGDIR                = "/pnfs/uboone/scratch/users/{USER}/pawel_dm_gen/{DECAY_CHAN}/run{RUN_NUMBER}/{mA}/log".format( USER = os.getenv("USER"), DECAY_CHAN = DECAY_CHAN, mA = MA_DIR, RUN_NUMBER = RUN_NUMBER)
TEMPLATE              = "/pnfs/uboone/resilient/users/anavrera/dark_tridents/templates/template_parameter_uboone_root_all.dat"
FILETAG               = ""
TARFILE_NAME          = "local_install_dark_tridents.tar.gz"


def get_options():
    parser       = optparse.OptionParser(usage="usage: %prog [options]")
    grid_group   = optparse.OptionGroup(parser, "Grid Options")

    grid_group.add_option("--make_tar", default = DO_TAR, help = "Make new tarfile. Default = False")
    grid_group.add_option("--outdir", default = OUTDIR, help = "Output flux histograms location. Default = %default.")
    grid_group.add_option("--logdir", default = LOGDIR, help = "Output log file location. Default = %default.")
    grid_group.add_option("--n_jobs", default = N_JOBS, type=int, help = "Number of g4numi jobs. Default = %default.")
    grid_group.add_option("--run_number", default = RUN_NUMBER, type=int, help = "Tag on the end of outfiles. Doubles as random # seed. Default = %default.")
    
    #grid_group.add_option('--pot',default = POT, type=int, help="Number of protons on target to simulate. Default = %default.")
    grid_group.add_option('--filetag', default = FILETAG)
    
    #uboone_group   = optparse.OptionGroup(parser, "uBoone Options")
    
    bdnmc_group    = optparse.OptionGroup(parser, "BdNMC Options")
    bdnmc_group.add_option('--template', default = TEMPLATE, help='Specify template parameter file. Default = parameter_uboone_template_grid_pi0.dat')
    bdnmc_group.add_option('--mA', default = MA_DIR, type=float, help='Specify dark photon mass. Default = 0.05.')
    bdnmc_group.add_option('--ratio', default = RATIO, type=float, help='Specify dark photon mass to dark matter mass ratio. Default = 0.6.')
    bdnmc_group.add_option('--nevts', default = NEVTS, type=int, help='Specify number of events to generate. Default = 5000.')
    bdnmc_group.add_option('--alD', default = ALD, help='Specify value of alD. Default = 0.1')
    bdnmc_group.add_option('--dm_type', default = DM_TYPE, help='Specify value of alD. Default = 0.1')
    bdnmc_group.add_option('--decay_channel', default = DECAY_CHAN, help='Specify meson decay channel (pi0_decay, eta_decay, all). Default = pi0_decay.')
    bdnmc_group.add_option('--signal_channel', default = SIGNAL, help='Specify signal channel. Default = NCE_nucleon.')

    parser.add_option_group(grid_group)
    parser.add_option_group(bdnmc_group)
    #parser.add_option_group(old_group)
    #parser.add_option_group(uboone_group)

    options, remainder = parser.parse_args()

    print("\nPrinting config options to run with: \n")
    print('mA',                        options.mA)
    print('mX',                        options.mA * options.ratio)
    print('run',                       options.run_number)

    return options

def make_parfile(options):
  template_filename = options.template
  template_string   = open(template_filename, 'r').read()
  template          = string.Template(template_string)

  macro_string = template.safe_substitute(
      {
        'run':                str(options.run_number),
        'nevents':            str(options.nevts),
        'dp_mass':            str(options.mA),
        'dm_mass':            str(options.mA * options.ratio),
        'alD':                str(options.alD),
        'decay_type':         options.dm_type
      }
    )

  macro_name = "parameter_uboone_grid.dat"
  macro = open(macro_name, "w") 
  macro.write(macro_string)
  macro.close()

  return macro_name

def make_tarfile(output_filename, mass):
    os.listdir(".")
    tar = tarfile.open(output_filename, "w:gz")
    tar.add("parameter_uboone_grid.dat")
    tar.add("mesons/pi0s.dat", arcname="pi0s.dat")
    tar.add("mesons/etas.dat", arcname="etas.dat")
    tar.add("BdNMC/bin/BDNMC")
    for i in os.listdir("BdNMC/build"):
      tar.add("BdNMC/build/"+i)
    for i in os.listdir("BdNMC/src"):
      tar.add("BdNMC/src/"+i)
    #tar.add("integral_fast.exe", arcname="integral_range.exe")
    #tar.add("xsec/cross_section_{}.root".format(mass), arcname="cross_section.root")
    tar.add("evgen_training.exe", arcname="evgen_rootinput.exe")
    tar.add("scripts/setup/setup_evgen_grid.sh", arcname="setup_evgen_grid.sh")
    tar.close()



def main():

    print("Job submission")

    options       = get_options()
    parfile       = make_parfile(options)
    MA_DIR        = options.mA
    SIGNAL        = options.signal_channel
    decay_channel = options.decay_channel
    template      = options.template
    make_tar      = options.make_tar
    DM_TYPE       = options.dm_type

    # Create a run number directory                                                                                                                                                                                                                                               
    RUNDIR = "/pnfs/uboone/scratch/users/{USER}/pawel_dm_gen/run{RUN_NUMBER}/{sig}/{mA}/{DECAY_CHAN}/".format( USER = os.getenv("USER"), DECAY_CHAN = decay_channel, mA = MA_DIR, RUN_NUMBER = options.run_number, sig = SIGNAL)
    print(RUNDIR)

    if os.path.isdir(RUNDIR) == False:
        print(RUNDIR, " directory doen't exist, so creating...\n")
        os.makedirs(RUNDIR)

    # Create a output file directory                                                                                                                                                                                                                                              
    OUTDIR = "/pnfs/uboone/scratch/users/{USER}/pawel_dm_gen/run{RUN_NUMBER}/{sig}/{mA}/{DECAY_CHAN}/{dm}/files/".format( USER = os.getenv("USER"), DECAY_CHAN = decay_channel, mA = MA_DIR, RUN_NUMBER = options.run_number, sig = SIGNAL, dm=DM_TYPE)

    if os.path.isdir(OUTDIR) == False:
        print(OUTDIR, " directory doen't exist, so creating...\n")
        os.makedirs(OUTDIR)

    # Create a log file directory                                                                                                                                                                                                                                                 
    LOGDIR = "/pnfs/uboone/scratch/users/{USER}/pawel_dm_gen/run{RUN_NUMBER}/{sig}/{mA}/{DECAY_CHAN}/{dm}/log/".format( USER = os.getenv("USER"), DECAY_CHAN = decay_channel, mA = MA_DIR, RUN_NUMBER = options.run_number, sig = SIGNAL, dm=DM_TYPE)

    if os.path.isdir(LOGDIR) == False:
        print(LOGDIR, " directory doen't exist, so creating...\n")
        os.makedirs(LOGDIR)
    
    # Create a cache file directory  
    CACHE_PNFS_AREA = "/pnfs/uboone/scratch/users/{USER}/pawel_dm_gen/run{RUN_NUMBER}/{sig}/{mA}/{DECAY_CHAN}/{dm}/CACHE/".format(USER = os.getenv("USER"), DECAY_CHAN = decay_channel, mA = MA_DIR, RUN_NUMBER = options.run_number, sig = SIGNAL, dm=DM_TYPE)

    if os.path.isdir(CACHE_PNFS_AREA) == False:
      print(CACHE_PNFS_AREA, " directory doen't exist, so creating...\n")
      os.makedirs(CACHE_PNFS_AREA)

    # scratch /pnfs area from which to send tarfile to grid                                                                                                                                                                                                                       
    cache_folder = CACHE_PNFS_AREA + str(random.randint(10000,99999)) + "/"
    os.makedirs(cache_folder)

    if(make_tar):
      print("\nTarring up local area...")
      make_tarfile(TARFILE_NAME, options.mA)  

    #always copy jobfile and tarball to cache area
    shutil.copy(TARFILE_NAME,    cache_folder)
    shutil.copy("/uboone/app/users/anavrera/pawel_dark_tridents/grid/dark_tridents_poms.sh", cache_folder)

    print("\nTarball of local area:", cache_folder + TARFILE_NAME)

    logfile = LOGDIR + "/dark_trident_{mA}_{RUN}_{TIME}_\$PROCESS.log".format(mA  = MA_DIR, RUN = options.run_number, TIME = time.strftime("%Y%m%d-%H%M%S"))

    print("\nOutput logfile(s):",logfile)

    submit_command = ("jobsub_submit {GRID} {MEMORY} {DISK} -N {NJOBS} -d DARKTRIDENT {OUTDIR} "
      "-G uboone "
      "-e RUN={RUN} "
      "-e MA={MASS} "
      "-e RATIO={RATIO} "
      "-e ALD={ALD} "
      "-e DM_TYPE={DMTYPE} "
      "-e NEVTS={NEVTS} "
      "-f {TARFILE} "
      "-L {LOGFILE} "
      "file://{CACHE}/dark_tridents_poms.sh".format(
      GRID       = ("--OS=SL7 -g "
                    "--resource-provides=usage_model=DEDICATED,OPPORTUNISTIC,OFFSITE "
                    "--role=Analysis "
                    "--expected-lifetime=2h "),
      DISK       = "--disk 0.5GB",
      MEMORY     = "--memory 0.5GB ",
      NJOBS      = options.n_jobs,
      OUTDIR     = OUTDIR,
      RUN        = options.run_number,
      MASS       = options.mA,
      RATIO      = options.ratio,
      ALD        = options.alD,
      DMTYPE     = options.dm_type,
      NEVTS      = options.nevts,
      TARFILE    = cache_folder + TARFILE_NAME,
      LOGFILE    = logfile,
      CACHE      = cache_folder)
  )

  #Ship it                                                                                                                                                                                                                                                                      
    print("\nSubmitting to grid:\n"+submit_command+"\n")
    status = subprocess.call(submit_command, shell=True)

if __name__ == "__main__":
    sys.exit(main())
    #ptions = get_options()
    #parfile = make_parfile(options)