cmd_/home/sandipan/x/lkm/Module.symvers := sed 's/\.ko$$/\.o/' /home/sandipan/x/lkm/modules.order | scripts/mod/modpost -m -a  -o /home/sandipan/x/lkm/Module.symvers -e -i Module.symvers   -T -
