# ====================================================================
#
#   Copyright (C) 2013 Qualcomm Atheros, Inc.
#   
#   Permission to use, copy, modify, and/or distribute this software 
#   for any purpose with or without fee is hereby granted, provided 
#   that the above copyright notice and this permission notice appear 
#   in all copies.
#   
#   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL 
#   WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED 
#   WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL  
#   THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR 
#   CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM 
#   LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, 
#   NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
#   CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#   
# --------------------------------------------------------------------

include $(TOPDIR)/rules.mk

# ====================================================================
# package identity constants;
# --------------------------------------------------------------------

PKG_NAME:=open-plc-utils
PKG_VERSION:=2013-01-29
PKG_RELEASE:=6beeb6fe6ce2b16b14284c26e1b9220b68044591

# ====================================================================
# package download constants;
# --------------------------------------------------------------------

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git://github.com/qca/open-plc-utils.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=$(PKG_RELEASE)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz

# ====================================================================
# package build constants;
# --------------------------------------------------------------------

# PKG_BUILD_PARALLEL:=1

# ====================================================================
#
# --------------------------------------------------------------------

include $(INCLUDE_DIR)/package.mk

# ====================================================================
#
# --------------------------------------------------------------------

define Package/open-plc-utils/default
 SECTION:=utils
 CATEGORY:=Utilities
 TITLE:=Qualcomm Atheros Powerline Communication utilities
 URL:=https://github.qualcomm.com/cmaier/open-plc-utils/blob/master/README
 MAINTAINER:=Qualcomm Atheros, Ocala FL USA
endef

# ====================================================================
#
# --------------------------------------------------------------------

include $(INCLUDE_DIR)/package.mk

# ====================================================================
#
# --------------------------------------------------------------------

define Package/open-plc-utils
 $(call Package/open-plc-utils/default)
 TITLE:=Qualcomm Atheros Powerline Toolkit
 MENU:=2
endef

# ====================================================================
#
# --------------------------------------------------------------------

define Package/open-plc-utils/description
 This package contains the PLC toolkit.
 It can be used to control PLC devices over the network.
endef

# ====================================================================
#
# --------------------------------------------------------------------

define Build/Compile
	$(TARGET_CONFIG_OPTS) $(MAKE) -C $(PKG_BUILD_DIR) CROSS=$(TARGET_CROSS)
endef

# ====================================================================
# define package program handler;
# --------------------------------------------------------------------

define AddPlcUtil
 define Package/open-plc-utils-$(2)
  $(call Package/open-plc-utils/default)
  TITLE:=$(3)
  DEPENDS+=open-plc-utils
 endef
 define Package/open-plc-utils-$(2)/description
  $(4)
 endef
 define Package/open-plc-utils-$(2)/install
	$(INSTALL_DIR) $$(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(1)/$(2) $$(1)/usr/bin
 endef
 $$(eval $$(call BuildPackage,open-plc-utils-$(2)))
endef

# ====================================================================
# call package program handler;
# --------------------------------------------------------------------

$(eval $(call AddPlcUtil,open-plc-utild,plc,efbu,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,efeu,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,efru,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,efsu,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,edru,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,edsu,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,nics,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,hpavkey,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,hpavkeys,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,rkey,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,mac2pw,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,mac2pwd,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,mdioblock,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,mdioblock2,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,mdiodump,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,mdiogen,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,hpav,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,mme,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,chknvm,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,chknvm2,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,nvmsplit,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,nvmmerge,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,chkpib,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,chkpib2,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,setpib,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,getpib,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,modpib,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,pib2xml,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,pibcomp,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,pibdump,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,pibruin,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,xml2pib,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,psin,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,psout,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,pskey,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,psgraph,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,psnotch,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,pibrump,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,int6k,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,int6kboot,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,int6keth,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,int6kf,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,int6khost,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,int64host,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,int6kid,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,int6klist,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,int6klog,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,int6kmdio,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,int6kmdio2,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,int6kmod,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,int6kstat,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,int6ktest,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,int6krate,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,int6krule,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,int6ktone,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,int6kwait,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,CMEncrypt,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,sada,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,coqos_add,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,coqos_info,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,coqos_man,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,coqos_mod,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,coqos_rel,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,mdustats,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,ampboot,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,amphost,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,ampID,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,amplist,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,amprate,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,ampstat,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,amptest,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,amptool,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,amptone,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,ampwait,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,plcboot,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,plchost,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,plcID,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,plclist,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,plcrate,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,plcrule,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,plcstat,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,plctest,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,plctool,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,plctone,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,plcwait,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,plchostd,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,plcget,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,plcset,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,plcotst,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,plcfwd,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,plcdevs,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,plclog,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,plcmdio16,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,plcmdio32,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,config2cfg,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,sdram,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,int6kuart,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,int6kbaud,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,ttysig,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,ptsctl,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,weeder,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,ttysend,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,ttyrecv,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,ttycat,'',''))
$(eval $(call AddPlcUtil,open-plc-utild,plc,int6kdetect,'',''))
# ====================================================================
#
# --------------------------------------------------------------------

$(eval $(call BuildPackage,open-plc-utils))

