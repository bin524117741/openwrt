#
# MT76x8 Profiles
#

include ./common-tp-link.mk

DEFAULT_SOC := mt7628an

define Build/elecom-header
	$(eval model_id=$(1))
	( \
		fw_size="$$(printf '%08x' $$(stat -c%s $@))"; \
		echo -ne "$$(echo "031d6129$${fw_size}06000000$(model_id)" | \
			sed 's/../\\x&/g')"; \
		dd if=/dev/zero bs=92 count=1; \
		data_crc="$$(dd if=$@ | gzip -c | tail -c 8 | \
			od -An -N4 -tx4 --endian little | tr -d ' \n')"; \
		echo -ne "$$(echo "$${data_crc}00000000" | sed 's/../\\x&/g')"; \
		dd if=$@; \
	) > $@.new
	mv $@.new $@
endef

define Build/ravpower-wd009-factory
	mkimage -A mips -T standalone -C none -a 0x80010000 -e 0x80010000 \
		-n "OpenWrt Bootloader" -d $(UBOOT_PATH) $@.new
	cat $@ >> $@.new
	@mv $@.new $@
endef







define Device/mediatek_linkit-smart-7688
  IMAGE_SIZE := 32448k
  DEVICE_VENDOR := MediaTek
  DEVICE_MODEL := LinkIt Smart 7688
  DEVICE_PACKAGES:= kmod-usb2 kmod-usb-ohci uboot-envtools kmod-sdhci-mt7620
  SUPPORTED_DEVICES += linkits7688 linkits7688d
endef
TARGET_DEVICES += mediatek_linkit-smart-7688

define Device/mediatek_mt7628an-eval-board
  BLOCKSIZE := 64k
  IMAGE_SIZE := 7872k
  DEVICE_VENDOR := MediaTek
  DEVICE_MODEL := MT7628 EVB
  DEVICE_PACKAGES := kmod-usb2 kmod-usb-ohci kmod-usb-ledtrig-usbport
  SUPPORTED_DEVICES += mt7628
endef
TARGET_DEVICES += mediatek_mt7628an-eval-board


