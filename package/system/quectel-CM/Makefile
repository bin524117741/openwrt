#
# Copyright (C) 2016 Rafał Miłecki <zajec5@gmail.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=quectel-CM
PKG_RELEASE:=1


include $(INCLUDE_DIR)/package.mk

define Package/quectel-CM
  SECTION:=utils
  CATEGORY:=Base system
  TITLE:=quectel-CM for EC20/BG96/BG95 call application
endef

define Package/quectel-CM/description
 This package contains an utility that allows handling Seama images.
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) -Wall"
endef

#$(INSTALL_DIR) $(1)/usr/lib
#$(CP) $(PKG_BUILD_DIR)/sdk/lib/libquectel-CM_gw_ext_sdk.a $(1)/usr/lib/
define Package/quectel-CM/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/quectel-CM $(1)/usr/bin/
endef

$(eval $(call BuildPackage,quectel-CM))