import pyRot, sys
tup = pyRot.rootTuple('merit.root', 'Exposure')
tup.readTree(())

for i in range(tup.nrows):
    sys.stdout.write("%.14e\t" % tup.time[i])
    sys.stdout.write("%10e\t" % tup.posx[i])
    sys.stdout.write("%10e\t" % tup.posy[i])
    sys.stdout.write("%10e\t" % tup.posz[i])
    sys.stdout.write("%10e\t" % tup.raz[i])
    sys.stdout.write("%10e\t" % tup.decz[i])
    sys.stdout.write("%10e\t" % tup.rax[i])
    sys.stdout.write("%10e\t" % tup.decx[i])
    #sys.stdout.write("%10e\t" % tup.razenith[i])
    #sys.stdout.write("%10e\t" % tup.deczenith[i])
    sys.stdout.write("%10e\t" % 0.)
    sys.stdout.write("%10e\t" % 0.)
    sys.stdout.write("%10e\t" % tup.lon[i])
    sys.stdout.write("%10e\t" % tup.lat[i])
    sys.stdout.write("%10e\n" % tup.alt[i])

