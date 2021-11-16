#
# Copyright (C) 2016 Rafał Miłecki <zajec5@gmail.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=cpp-demo
PKG_RELEASE:=1


include $(INCLUDE_DIR)/package.mk

define Package/cpp-demo
  SECTION:=utils
  CATEGORY:=Base system
  TITLE:=Utility for handling Seama firmware images
endef

define Package/cpp-demo/description
 This package contains an utility that allows handling Seama images.
endef

define Package/cpp-demo/extra_provides
    echo 'libstdc++.so.6'; \
    echo 'libm.so.6'; \
    echo 'libc.so.6';
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
        CXX="$(TARGET_CXX)" \
		CFLAGS="$(TARGET_CFLAGS) -Wall"
endef

define Package/cpp-demo/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/cpp-demo $(1)/usr/bin/
endef

$(eval $(call BuildPackage,cpp-demo))