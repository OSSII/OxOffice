# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the LibreOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

$(eval $(call gb_CustomTarget_CustomTarget,extras/fonts))

$(eval $(call gb_CustomTarget_register_targets,extras/fonts, \
	SourceHanSansTC-Regular.otf \
	SourceHanSansTC-Bold.otf \
	SourceHanSansHWTC-Regular.otf \
	SourceHanSansHWTC-Bold.otf \
	SourceHanSerifTC-Regular.otf \
	SourceHanSerifTC-Bold.otf \
	TW-Kai-98_1.ttf \
	TW-Kai-Ext-B-98_1.ttf \
	TW-Sung-98_1.ttf \
	TW-Sung-Ext-B-98_1.ttf \
	OxWingdings-Regular.ttf \
	OxWingdings2-Regular.ttf \
	OxWingdings3-Regular.ttf \
))

$(call gb_CustomTarget_get_workdir,extras/fonts)/SourceHanSansTC-Regular.otf : \
		$(SRCDIR)/extras/source/truetype/SourceHanSans/SourceHanSansTC-Regular.otf
	cp $< $@

$(call gb_CustomTarget_get_workdir,extras/fonts)/SourceHanSansTC-Bold.otf : \
		$(SRCDIR)/extras/source/truetype/SourceHanSans/SourceHanSansTC-Bold.otf
	cp $< $@

$(call gb_CustomTarget_get_workdir,extras/fonts)/SourceHanSansHWTC-Regular.otf : \
		$(SRCDIR)/extras/source/truetype/SourceHanSans/SourceHanSansHWTC-Regular.otf
	cp $< $@

$(call gb_CustomTarget_get_workdir,extras/fonts)/SourceHanSansHWTC-Bold.otf : \
		$(SRCDIR)/extras/source/truetype/SourceHanSans/SourceHanSansHWTC-Bold.otf
	cp $< $@

$(call gb_CustomTarget_get_workdir,extras/fonts)/SourceHanSerifTC-Regular.otf : \
		$(SRCDIR)/extras/source/truetype/SourceHanSerif/SourceHanSerifTC-Regular.otf
	cp $< $@

$(call gb_CustomTarget_get_workdir,extras/fonts)/SourceHanSerifTC-Bold.otf : \
		$(SRCDIR)/extras/source/truetype/SourceHanSerif/SourceHanSerifTC-Bold.otf
	cp $< $@

$(call gb_CustomTarget_get_workdir,extras/fonts)/TW-Kai-98_1.ttf : \
		$(SRCDIR)/extras/source/truetype/cns11643/TW-Kai-98_1.ttf
	cp $< $@

$(call gb_CustomTarget_get_workdir,extras/fonts)/TW-Kai-Ext-B-98_1.ttf : \
		$(SRCDIR)/extras/source/truetype/cns11643/TW-Kai-Ext-B-98_1.ttf
	cp $< $@

$(call gb_CustomTarget_get_workdir,extras/fonts)/TW-Sung-98_1.ttf : \
		$(SRCDIR)/extras/source/truetype/cns11643/TW-Sung-98_1.ttf
	cp $< $@

$(call gb_CustomTarget_get_workdir,extras/fonts)/TW-Sung-Ext-B-98_1.ttf : \
		$(SRCDIR)/extras/source/truetype/cns11643/TW-Sung-Ext-B-98_1.ttf
	cp $< $@

$(call gb_CustomTarget_get_workdir,extras/fonts)/OxWingdings-Regular.ttf : \
		$(SRCDIR)/extras/source/truetype/OxSymbol/OxWingdings-Regular.ttf
	cp $< $@

$(call gb_CustomTarget_get_workdir,extras/fonts)/OxWingdings2-Regular.ttf : \
		$(SRCDIR)/extras/source/truetype/OxSymbol/OxWingdings2-Regular.ttf
	cp $< $@

$(call gb_CustomTarget_get_workdir,extras/fonts)/OxWingdings3-Regular.ttf : \
		$(SRCDIR)/extras/source/truetype/OxSymbol/OxWingdings3-Regular.ttf
	cp $< $@

# vim: set noet sw=4 ts=4:
