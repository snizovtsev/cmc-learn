TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = common learn predict rate

learn.depends = common
predict.depends = common
rate.depends = common
