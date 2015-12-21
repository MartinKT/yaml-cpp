TEMPLATE = subdirs

SUBDIRS = parse read sandbox

for(subdir, SUBDIRS) {
  $${subdir}.file = $${subdir}.pro
  $${subdir}.makefile = Makefile.$${subdir}
}

