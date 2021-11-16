#
# Copyright (C) 2016 Rafał Miłecki <zajec5@gmail.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=helloword
PKG_RELEASE:=1


include $(INCLUDE_DIR)/package.mk

define Package/helloword
  SECTION:=utils
  CATEGORY:=Base system
  TITLE:=Utility for handling Seama firmware images
endef

define Package/helloword/description
 This package contains an utility that allows handling Seama images.
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) -Wall"
endef

define Package/helloword/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/helloword $(1)/usr/bin/
endef

$(eval $(call BuildPackage,helloword))