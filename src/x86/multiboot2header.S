/*
 * Define constants for the multiboot header. See Multuboot 2 Specifications for details.
 */
.set align, 1<<0
.set meminfo, 1<<1
.set magic, 0xE85250D6
.set arch, 0
.set headerLength, _multibootHeaderEnd - _multibootHeaderStart
.set checksum, -(magic + arch + headerLength)

.set tagNotOptional, 0

.set tagInfoRequestType, 1
.set tagInfoRequestSize, _multibootInfoTagEnd - _multibootInfoTagStart
.set requestBootCommand, 1
.set requestBootLoaderName, 2
.set requestBootModules, 3
.set requestMemoryInfo, 4
.set requestBootDevice, 5
.set requestMemoryMap, 6

.set tagAddressType, 2
.set tagAddressSize, 24
.set tagAddressHeaderLocation, LOAD_START
.set tagAddressLoadStart, LOAD_START
.set tagAddressLoadEnd, LOAD_END
.set tagAddressBSSEnd, BSS_END

.set tagEntryType, 3
.set tagEntrySize, 12
.set tagEntryAddress, _start - (0xFF900000 - 0x100000)

.set tagModuleAlignType, 6
.set tagModuleAlignSize, 8

/*
 * Each multiboot tag must be 8-byte aligned, or GRUB will not be able to read the header.
 */
.align 8
_multibootHeaderStart:

.long magic
.long arch
.long headerLength
.long checksum

.align 8

_multibootInfoTagStart:
.short tagInfoRequestType
.short tagNotOptional
.long tagInfoRequestSize
.long requestBootCommand
.long requestBootLoaderName
.long requestBootModules
.long requestMemoryInfo
.long requestBootDevice
.long requestMemoryMap
_multibootInfoTagEnd:

.align 8

.short tagAddressType
.short tagNotOptional
.long tagAddressSize
.long tagAddressHeaderLocation
.long tagAddressLoadStart
.long tagAddressLoadEnd
.long tagAddressBSSEnd

.align 8

.short tagEntryType
.short tagNotOptional
.long tagEntrySize
.long tagEntryAddress

.align 8

.short tagModuleAlignType
.short tagNotOptional
.long tagModuleAlignSize

.align 8

/*
 * Terminate list of multiboot header tags.
 * Ending tag has type = 0, flags = 0, size = 8
 */
.long 0
.long 8

_multibootHeaderEnd:

