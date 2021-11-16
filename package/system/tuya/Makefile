#
# Copyright (C) 2016 Rafał Miłecki <zajec5@gmail.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=tuya
PKG_RELEASE:=1


include $(INCLUDE_DIR)/package.mk

define Package/tuya
  SECTION:=utils
  CATEGORY:=Base system
  TITLE:=Utility for handling Seama firmware images
endef

define Package/tuya/description
 This package contains an utility that allows handling Seama images.
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) -Wall"
endef

#$(INSTALL_DIR) $(1)/usr/lib
#$(CP) $(PKG_BUILD_DIR)/sdk/lib/libtuya_gw_ext_sdk.a $(1)/usr/lib/
define Package/tuya/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/user_iot $(1)/usr/bin/
endef

$(eval $(call BuildPackage,tuya))