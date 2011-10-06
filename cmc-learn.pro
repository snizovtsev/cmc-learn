TEMPLATE = subdirs

SUBDIRS = common learn predict

learn.depends = common
predict.depends = common
