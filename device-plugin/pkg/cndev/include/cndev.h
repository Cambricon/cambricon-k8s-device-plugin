#ifndef INCLUDE_CNDEV_H_
#define INCLUDE_CNDEV_H_

#ifndef WIN32
#include <libgen.h>
#include <linux/pci_regs.h>
#include <unistd.h>
#endif
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#if defined(__cplusplus)
extern "C" {
#endif /*__cplusplus*/

// api versions
#define CNDEV_VERSION_1 1
#define CNDEV_VERSION_2 2
#define CNDEV_VERSION_3 3
#define CNDEV_VERSION_4 4
#define CNDEV_VERSION_5 5
#define CNDEV_VERSION_6 6

#ifndef MLULINK_PORT
#define MLULINK_PORT 8
#endif
#define CNDEV_MLULINK_PORT 8

#ifndef TINYCOREMAXCOUNT
#define TINYCOREMAXCOUNT 10
#endif
#define CNDEV_TINYCOREMAXCOUNT 10

#ifndef MAX_CORE_COUNT
#define MAX_CORE_COUNT 32
#endif
#define CNDEV_MAX_CORE_COUNT 32       /**< The max CPU core count.*/

#define CNDEV_MLUINSTANCE_PROFILE_NAME_BUFFER_SIZE 100
#define CNDEV_MLUINSTANCE_DEVICE_NODE_NAME_BUFFER_SIZE 100
#define CNDEV_NAME_BUFFER_SIZE 200
#define CNDEV_MLU_CLUSTER_FREQUENCY_COUNT 20
typedef __int32_t cndevDevice_t;        /**< Defines \p cndevDevice_t as \p __int32, used for device handle of CNDev.*/
typedef __int32_t cndevMluInstance_t;   /**< Defines \p cndevMluInstance_t as \p __int32_t, used for MLU instance of CNDev.*/ 

#if defined(__DOXYGEN_ONLY__)
#define CNDEV_EXPORT
#else
#if defined(WIN32) || defined(WINDOWS)
#define CNDEV_EXPORT __declspec(dllexport)
#elif defined(LINUX) || defined(__linux) || defined(__CYGWIN__)
#define CNDEV_EXPORT __attribute__((visibility("default")))
#endif
#endif

typedef unsigned char __uint8_t;      /**< Defines \p __uint8_t as ``unsigned char``.*/
typedef unsigned short __uint16_t;    /**< Defines \p __uint16_t as ``unsigned short``.*/
typedef int __int32_t;                /**< Defines \p __uint32_t as ``int``.*/
typedef unsigned int __uint32_t;      /**< Defines \p __uint32_t as ``unsigned int``.*/
typedef unsigned long __uint64_t;     /**< Defines \p __uint64_t as ``unsigned long``.*/
typedef long __int64_t;               /**< Defines \p __uint64_t as ``long``.*/

/**
 * @brief The return values for CNDev APIs.
 */
typedef enum {
  /**
   * This indicates that the API has been executed successfully.
   */
  CNDEV_SUCCESS = 0,
  /**
   * This indicates that MLU Driver is not loaded.
   */
  CNDEV_ERROR_NO_DRIVER = 1,
  /**
   * Indicates the version of MLU Driver is below the minimum supported level.
   */
  CNDEV_ERROR_LOW_DRIVER_VERSION = 2,
  /**
   * Indicates an unsupported or invalid version for this API.
   */
  CNDEV_ERROR_UNSUPPORTED_API_VERSION = 3,
  /**
   * This indicates that CNDev has not been initialized.
   */
  CNDEV_ERROR_UNINITIALIZED = 4,
  /**
   * Indicates invalid value for parameter.
   */
  CNDEV_ERROR_INVALID_ARGUMENT = 5,
  /**
   This indicates that the device handle is invalid.
   */
  CNDEV_ERROR_INVALID_DEVICE_ID = 6,
  /**
   This indicates that system function occur error.
   */
  CNDEV_ERROR_UNKNOWN = 7,
  /**
   This indicates the memory allocation failure.
   */
  CNDEV_ERROR_MALLOC = 8,
  /**
   This indicates that CNDev has not enough space.
   */
  CNDEV_ERROR_INSUFFICIENT_SPACE = 9,
  /**
   This indicates that the operation is not supported.
   */
  CNDEV_ERROR_NOT_SUPPORTED = 10,
  /**
   This indicates that the link port is invalid.
   */
  CNDEV_ERROR_INVALID_LINK = 11,
  /**
   * This indicates that MLU Driver cannot detect any usable MLU device.
   */
  CNDEV_ERROR_NO_DEVICES = 12,
  /**
   This indicates that current user does not have permission.
   */
  CNDEV_ERROR_NO_PERMISSION = 13,
  /**
   This indicates that searching is failed.
   */
  CNDEV_ERROR_NOT_FOUND = 14,
  /**
   This indicates that an operation cannot be performed because MLU is currently in use.
   */
  CNDEV_ERROR_IN_USE = 15,
  /**
   This indicates that the instance name is duplicate.
   */
  CNDEV_ERROR_DUPLICATE = 16,
  /**
   This indicates that a timeout occurs.
   */
  CNDEV_ERROR_TIMEOUT = 17,
} cndevRet_t;

/** Device names.*/
typedef enum {
  CNDEV_DEVICE_TYPE_MLU100 = 0,                /**< The device is MLU100.*/
  CNDEV_DEVICE_TYPE_MLU270 = 1,                /**< The device is MLU270.*/
  CNDEV_DEVICE_TYPE_MLU220_M2 = 16,            /**< The device is MLU220_M2.*/
  CNDEV_DEVICE_TYPE_MLU220_EDGE = 17,          /**< The device is MLU220_EDGE.*/
  CNDEV_DEVICE_TYPE_MLU220_EVB = 18,           /**< The device is MLU220_EVB.*/
  CNDEV_DEVICE_TYPE_MLU220_M2i = 19,           /**< The device is MLU220_M2i.*/
  CNDEV_DEVICE_TYPE_MLU290 = 20,               /**< The device is MLU290.*/
  CNDEV_DEVICE_TYPE_MLU370 = 23,               /**< The device is MLU370.*/
  CNDEV_DEVICE_TYPE_MLU365 = 24,               /**< The device is MLU365.*/
  CNDEV_DEVICE_TYPE_CE3226 = 25,               /**< The device is CE3226.*/
  CNDEV_DEVICE_TYPE_MLU590 = 26,               /**< The device is MLU590.*/
  CNDEV_DEVICE_TYPE_MLU585 = 27,               /**< The device is MLU585.*/
  CNDEV_DEVICE_TYPE_1V_2201 = 29,              /**< The device is 1V_2201.*/
  CNDEV_DEVICE_TYPE_MLU580 = 30,               /**< The device is MLU580.*/
  CNDEV_DEVICE_TYPE_MLU570 = 31,               /**< The device is MLU570.*/
  CNDEV_DEVICE_TYPE_1V_2202 = 32,              /**< The device is 1V_2202.*/
  MLU100 = CNDEV_DEVICE_TYPE_MLU100,           /**< The device is MLU100.*/
  MLU270 = CNDEV_DEVICE_TYPE_MLU270,           /**< The device is MLU270.*/
  MLU220_M2 = CNDEV_DEVICE_TYPE_MLU220_M2,     /**< The device is MLU220_M2.*/
  MLU220_EDGE = CNDEV_DEVICE_TYPE_MLU220_EDGE, /**< The device is MLU220_EDGE.*/
  MLU220_EVB = CNDEV_DEVICE_TYPE_MLU220_EVB,   /**< The device is MLU220_EVB.*/
  MLU220_M2i = CNDEV_DEVICE_TYPE_MLU220_M2i,   /**< The device is MLU220_M2i.*/
  MLU290 = CNDEV_DEVICE_TYPE_MLU290,           /**< The device is MLU290.*/
  MLU370 = CNDEV_DEVICE_TYPE_MLU370,           /**< The device is MLU370.*/
  MLU365 = CNDEV_DEVICE_TYPE_MLU365,           /**< The device is MLU365.*/
  CE3226 = CNDEV_DEVICE_TYPE_CE3226,           /**< The device is CE3226.*/
  MLU590 = CNDEV_DEVICE_TYPE_MLU590,           /**< The device is MLU590.*/
  MLU585 = CNDEV_DEVICE_TYPE_MLU585,           /**< The device is MLU585.*/
  MLU580 = CNDEV_DEVICE_TYPE_MLU580,           /**< The device is MLU580.*/
  MLU570 = CNDEV_DEVICE_TYPE_MLU570,           /**< The device is MLU570.*/
} cndevNameEnum_t;

/** Topology relationship types.*/
typedef enum {
  SELF = 0,        /**< Same devices.*/ 
  INTERNAL = 1,    /**< Devices that are on the same board.*/
  SINGLE = 2,      /**< All devices that only need to traverse a single PCIe switch.*/
  MULTIPLE = 3,    /**< All devices that need not traverse a host bridge.*/
  HOST_BRIDGE = 4, /**< All devices that are connected to the same host bridge.*/
  CPU = 5,         /**< All devices that are connected to the same CPU but possibly multiple host bridges.*/
  SYSTEM = 6,      /**< All devices in the system.*/
} cndevTopologyRelationshipEnum_t;

/** Display formats for MLU-Link speed.*/
typedef enum {
  SPEED_FMT_NRZ = 0,    /**< Transmission speed show by NRZ format.*/
  SPEED_FMT_PM4 = 1,    /**< Transmission speed show by PM4 format.*/
} cndevMLULinkSpeedEnum_t;

/** @brief @deprecated Count types for MLU-Link.*/
typedef enum {
  MLULINK_CNTR_RD_BYTE = 0,                 /**< The count of read bytes.*/
  MLULINK_CNTR_RD_PKG = 1,                  /**< The count of read packages.*/
  MLULINK_CNTR_WR_BYTE = 2,                 /**< The count of write bytes.*/
  MLULINK_CNTR_WR_PKG = 3,                  /**< The count of write packages.*/
  MLULINK_ERR_RPY = 4,                      /**< The count of replay packages.*/
  MLULINK_ERR_FTL = 5,                      /**< The count of failures.*/
  MLULINK_ERR_ECC_DBE = 6,                  /**< The count of ECC Double errors.*/
  MLULINK_ERR_CRC24 = 7,                    /**< The count of CRC24 errors.*/
  MLULINK_ERR_CRC32 = 8,                    /**< The count of CRC32 errors.*/
  MLULINK_ERR_CORR = 9,                     /**< The count of corrected errors.*/
  MLULINK_ERR_UNCORR = 10,                  /**< The count of uncorrected errors.*/
  MLULINK_ERR_READ_PACKAGE = 11,            /**< The count of failed attempts to read packages.*/
  MLULINK_ERR_WRITE_PACKAGE = 12,           /**< The count of failed attempts to write packages.*/
  MLULINK_ERR_ILLEGAL_ACCESS_PACKAGE = 13,  /**< The count of illegal access.*/
  MLULINK_CNTR_CNP_PKGS = 14,               /**< The count of Congestion Notification Packages.*/
  MLULINK_CNTR_PFC_PKGS = 15,               /**< The count of Priority Flow Control packages.*/
  MLULINK_COUNTER_COUNT,                    /**< The total number of count types.*/
} cndevMLULinkCounterEnum_t;

/** Enabling status.*/
typedef enum {
  CNDEV_FEATURE_DISABLED = 0,  /**< Disabled state for a feature.*/
  CNDEV_FEATURE_ENABLED =  1   /**< Enabled state for a feature.*/
} cndevEnableStatusEnum_t;

/** MLU profile types.*/
typedef enum {
	CNDEV_MLUINSTANCE_PROFILE_1_SLICE,                  /**< Slices into 1 MLU cluster.*/
	CNDEV_MLUINSTANCE_PROFILE_1_SLICE_MLU_2_SLICE_VPU,  /**< Slices into 1 MLU cluster and 2 VPUs.*/
	CNDEV_MLUINSTANCE_PROFILE_2_SLICE,                  /**< Slices into 2 MLU clusters.*/
	CNDEV_MLUINSTANCE_PROFILE_2_SLICE_MLU_1_SLICE_MEM,  /**< Slices into 2 MLU clusters and 1 Memory Module.*/
	CNDEV_MLUINSTANCE_PROFILE_3_SLICE,                  /**< Slices into 3 MLU clusters.*/
	CNDEV_MLUINSTANCE_PROFILE_4_SLICE,                  /**< Slices into 4 MLU clusters.*/
	CNDEV_MLUINSTANCE_PROFILE_5_SLICE,                  /**< Slices into 5 MLU clusters.*/
	CNDEV_MLUINSTANCE_PROFILE_6_SLICE,                  /**< Slices into 6 MLU clusters.*/
	CNDEV_MLUINSTANCE_PROFILE_COUNT,                    /**< The total number of profile types.*/
} cndevMLUInstanceProfileEnum_t;

/** Configuration types.*/
typedef enum {
  CNDEV_DEVICE_CONFIGS_WATCHDOG_TIMER = 0,         /**< Configure watchdog timer in seconds. It must be in the range [0, 1023].*/
  CNDEV_DEVICE_CONFIGS_FAST_ALLOC = 1,             /**< Configure fast allocation function. Set 0 to disable and 1 to enable.*/
  CNDEV_DEVICE_CONFIGS_COMPRESS_MEM = 2,           /**< Configure compress function. Set 0 to disable and 1 to enable.*/
  CNDEV_DEVICE_CONFIGS_LINEAR_MEM = 3,             /**< Configure linear memory. Set 0 to disable and 2 to enable.*/
  CNDEV_DEVICE_CONFIGS_IPU_LLC_DATA_ALLOCATED = 4, /**< Configure control data cache mode in LLC for IPU. Set 0 to operate without allocating memory and 1 to allocate.*/
  CNDEV_DEVICE_CONFIGS_STOP_CODEC_XPU = 5,         /**< Configure codec server shutdown. Value 0 will shut down the codec server of current device.*/
  CNDEV_DEVICE_CONFIGS_PINNED_MEM_NUMA_MODE = 6,   /**< Configure host memory allocation with pinned NUMA node. Set 0 to disable and 1 to enable.*/
} cndevDeviceConfigsTypeEnum_t;

/**
 * @brief Gets the error message.
 * 
 * This API takes an error ID as input and returns the corresponding error message if found.
 * If the error message cannot be found, it returns "Cndev_ERROR not found!".
 *
 * @param[in] errorId The error ID.
 *
 * @return
 * The corresponding error message if found, otherwise "Cndev_ERROR not found!".
 */
CNDEV_EXPORT
const char *cndevGetErrorString(cndevRet_t errorId);

#ifdef WIN32
#define basename(file) "UNSUPPORTED"
#endif

#ifndef cndevCheckErrors
#define __cndevCheckErrors(err, file, line)                                                                                    \
  do {                                                                                                                         \
    cndevRet_t _err = (err);                                                                                                   \
    if (CNDEV_SUCCESS != _err) {                                                                                               \
      fprintf(stderr, "cndevCheckErrors(%d): %s, from file <%s>, line %i.\n",                                                  \
      _err, cndevGetErrorString(_err), basename((char*)file), line);                                                           \
      exit(1);                                                                                                                 \
    }                                                                                                                          \
  } while (0)
#define cndevCheckErrors(err) __cndevCheckErrors((err), __FILE__, __LINE__)
#endif
#ifndef UUID_SIZE
#define UUID_SIZE 37
#endif
#define CNDEV_UUID_SIZE 37
#ifndef IP_ADDRESS_LEN
#define IP_ADDRESS_LEN 40
#endif
#define CNDEV_IP_ADDRESS_LEN                   (40)
#define CNDEV_MLU_REMOTE_PORT_NAME_LEN         (256)
#define CNDEV_MAC_ADDRESS_LEN                  (6)

/** Card information.*/
typedef struct {
  int version;     /**< API version.*/
  unsigned number; /**< The count of devices.*/
} cndevCardInfo_t;

/** UUID information.*/
typedef struct {
  int version;                      /**< API version.*/
  __uint8_t uuid[CNDEV_UUID_SIZE];  /**< The information of the device UUID.*/
  __uint64_t ncsUUID64;             /**< The information of the ncs device UUID.*/
} cndevUUID_t;

/** Memory information.*/
typedef struct {
  int version;                      /**< API version.*/
  __int64_t physicalMemoryTotal;    /**< The physical total memory size of MLU, unit: MB.*/
  __int64_t physicalMemoryUsed;     /**< The physical used memory size of MLU, unit: MB.*/
  __int64_t virtualMemoryTotal;     /**< The virtual total memory size of MLU, unit: MB.*/
  __int64_t virtualMemoryUsed;      /**< The virtual used memory size of MLU, unit: MB.*/
  __int64_t channelNumber;          /**< The channel number.*/
  __int64_t channelMemoryUsed[20];  /**< The used memory size of each channel, the valid array index range [0, ``channelNumber`` - 1], unit: MB.*/
  __uint64_t globalMemory;          /**< The SMT memory size, unit: MB.*/
} cndevMemoryInfo_t;

/** Memory information V2.*/
typedef struct {
  __int64_t physicalMemoryTotal;    /**< The physical total memory size of MLU, unit: MB.*/
  __int64_t physicalMemoryUsed;     /**< The physical used memory size of MLU, unit: MB.*/
  __int64_t reservedMemory;         /**< The reserved memory size of MLU, unit: MB.*/
  __int64_t virtualMemoryTotal;     /**< The virtual total memory size of MLU, unit: MB.*/
  __int64_t virtualMemoryUsed;      /**< The virtual used memory size of MLU, unit: MB.*/
  __uint64_t globalMemory;          /**< The SMT memory size, unit: MB.*/
  __uint64_t reserved[16];          /**< Reserved.*/
} cndevMemoryInfoV2_t;

/** Version information.*/
typedef struct {
  int version;                 /**< API version.*/
  unsigned mcuMajorVersion;    /**< The major version number of the MCU.*/
  unsigned mcuMinorVersion;    /**< The minor version number of the MCU.*/
  unsigned mcuBuildVersion;    /**< The patch version number of the MCU.*/
  unsigned driverMajorVersion; /**< The major version number of MLU Driver.*/
  unsigned driverMinorVersion; /**< The minor version number of MLU Driver.*/
  unsigned driverBuildVersion; /**< The build version number of MLU Driver.*/
} cndevVersionInfo_t;

/** ECC information.*/
typedef struct {
  int version;                       /**< API version.*/
  __uint64_t oneBitError;            /**< The count of single single-bit errors.*/
  __uint64_t multipleOneError;       /**< The count of multiple single-bit errors.*/
  __uint64_t multipleError;          /**< The count of single multiple-bits errors.*/
  __uint64_t multipleMultipleError;  /**< The count of multiple multiple-bits errors.*/
  __uint64_t correctedError;         /**< The count of corrected errors ( ``oneBitError`` + ``multipleOneError``).*/
  __uint64_t uncorrectedError;       /**< The count of uncorrected errors ( ``multipleError`` + ``multipleMultipleError``).*/
  __uint64_t totalError;             /**< Total ECC errors ( ``correctedError`` + ``uncorrectedError``).*/
  __uint64_t addressForbiddenError;  /**< The count of access illegal addresses.*/
} cndevECCInfo_t;

/** ECC mode information.*/
typedef struct {
  int version;                    /**< API version.*/
  cndevEnableStatusEnum_t mode;   /**< The status of the ECC mode.*/
} cndevEccMode_t;

/** Power information.*/
typedef struct {
  int version;                                /**< API version.*/
  int usage;                                  /**< Current power dissipation of MLU device, unit: W.*/
  int cap;                                    /**< Current set power cap of MLU device, unit: W.*/
  int usageDecimal;                           /**< Decimal places for current power dissipation of MLU device, unit: W.*/
  int machine;                                /**< Current power dissipation of OAM machine, unit: W.*/
  int capDecimal;                             /**< Decimal places for current set power cap of MLU device, unit: W.*/
  __uint16_t thermalDesignPower;              /**< Thermal design power of MLU device, unit: W.*/
  __uint16_t instantaneousPowerUsage;         /**< Instantaneous power dissipation of MLU device, unit: W.*/
  __uint16_t instantaneousPowerUsageDecimal;  /**< Decimal places for instantaneous power dissipation of MLU device, unit: W.*/
} cndevPowerInfo_t;


/** Power information.*/
typedef struct {
  int usage;                                  /**< Current power dissipation of MLU device, unit: W.*/
  int cap;                                    /**< Current set power cap of MLU device, unit: W.*/
  int machine;                                /**< Current power dissipation of OAM machine, unit: W.*/
  __uint16_t thermalDesignPower;              /**< Thermal design power of MLU device, unit: W.*/
  __uint16_t instantaneousPowerUsage;         /**< Reserved.*/
  __uint16_t maxPower;                        /**< Maximum power dissipation of MLU device for customer evaluation of power supply, unit: W.*/
  int reserved[16];                           /**< Reserved.*/
} cndevPowerInfoV2_t;

/** Temperature information.*/
typedef struct {
  int version;       /**< API version.*/
  /**< Board temperature, unit: ℃. Collects statistics on multiple temperature information
  on the board and reports the maximum value.*/
  int board;
  /** < Cluster temperature, unit: ℃. To count the temperature of each IPU Core PVT sensor, take the maximum value and report it.
  In the case of Full MLU, the effective length is 1. Only in the case of
  Multi-Instance MLU virtualization, multiple temperature information will be returned.*/
  int cluster[20];
  int memoryDie[8];/** < reserved.*/
  /** < Chip top temperature, unit: ℃. To collect statistics on multiple temperature information of Chip Top,
   the maximum value is reported.*/
  int chip;
  int airInlet;   /**< The air inlet temperature of the whole machine transmitted by BMC, unit: ℃*/
  int airOutlet;  /**< The air outlet temperature of the whole machine transmitted by BMC, unit: ℃.*/
  int memory;     /**< The memory temperature, unit: ℃. To count the temperature of each HBM Sys, the maximum value is reported.*/
  int videoInput; /**< The temperature of video input, unit: ℃. Only edge products are valid.*/
  int cpu;        /**< The temperature of CPU, unit: ℃. Only edge products are valid.*/
  int isp;        /**< The temperature of ISP (Image Signal Processor), unit: ℃. Only edge products are valid.*/
} cndevTemperatureInfo_t;

/** Fan speed information.*/
typedef struct {
  int version;          /**< API version.*/
  int fanSpeed;         /**< Reserved.*/
  int chassisFanCount;  /**< The count of chassis fans, only valid on OAM machine.*/
  /**< The speed information of each chassis fan, unit: rpm, only valid on OAM machine,
   chassisFan[0]-chassisFan[19] are 8086 fan, chassisFan[20]-chassisFan[27] are 4028 fan.*/
  int chassisFan[28];
} cndevFanSpeedInfo_t;

/** MLU utilization information.*/
typedef struct {
  int version;                /**< API version.*/
  int averageCoreUtilization; /**< The average utilization of MLU core.*/
  int coreUtilization[80];    /**< The utilization of each MLU core, the valid array index range [0, MLU core - 1].*/
} cndevUtilizationInfo_t;

/** MLU frequency information.*/
typedef struct {
  int version;               /**< API version.*/
  int boardFreq;             /**< The current operating IPU frequency, unit: MHz.*/
  int ddrFreq;               /**< MLU DDR transfer rate, unit: MBps (operating max MLU DDR frequency, unit: MHz).*/
  __uint8_t overtempDfsFlag; /**< Over-temperature reducing frequency flag, only valid on MLU220.*/
  __uint8_t fastDfsFlag;     /**< Fast reducing frequency flag, only valid on MLU220.*/
  __uint16_t mluClusterFreqCount;       /**< The count of MLU clusters.*/
  /**< The frequency of each MLU cluster, the valid array index range [0, ``mluClusterFreqCount`` - 1], unit: MHz.*/
  __uint16_t mluClusterFreq[CNDEV_MLU_CLUSTER_FREQUENCY_COUNT]; 
  int boardDefaultFreq;      /**< IPU default frequency, unit: MHz.*/
  /**< The range of adjustable operating frequency, boardFreqArange[0] means the minimum operating frequency,
   * boardFreqArange[1] means the maximum operating frequency unit: MHz.*/
  int boardFreqArange[2];
} cndevFrequencyInfo_t;

/** Process information.*/
typedef struct {
  int version;                   /**< API version.*/
  unsigned int pid;              /**< Process ID.*/
  __uint64_t physicalMemoryUsed; /**< MLU physical memory used by the process, unit: KiB.*/
  __uint64_t virtualMemoryUsed;  /**< MLU virtual memory used by the process, unit: KiB.*/
} cndevProcessInfo_t;

/** Library version information.*/
typedef struct {
  int version;              /**< API version.*/
  unsigned libMajorVersion; /**< Library major version.*/
  unsigned libMinorVersion; /**< Library minor version.*/
  unsigned libBuildVersion; /**< Library build version.*/
} cndevLibVersionInfo_t;

/** Card core count.*/
typedef struct {
  int version; /**< API version.*/
  int count;   /**< Core count of devices.*/
} cndevCardCoreCount_t;

/** Card cluster count.*/
typedef struct {
  int version; /**< API version.*/
  int count;   /**< Cluster count of devices.*/
} cndevCardClusterCount_t;

/** Card name.*/
typedef struct {
  int version;        /**< API version.*/
  cndevNameEnum_t id; /**< Card name.*/
} cndevCardName_t;

/** Serial number and related information.*/
typedef struct {
  int version;                /**< API version.*/
  __uint64_t sn;              /**< Card's serial number in hexadecimal format.*/
  __uint64_t motherBoardSn;   /**< Motherboard's serial number in hexadecimal format.*/
} cndevCardSN_t;

/** Part number information of card.*/
typedef struct {
  int version;                /**< API version.*/
  char partNumber[32];        /**< Part number of card.*/
} cndevCardPartNumber_t;

/** Device form.*/
typedef struct {
  int version;                /**< API version.*/
  __uint64_t device_form;     /**< The form of device.*/
} cndevCardForm_t;

/** Device ID information.*/
typedef struct {
  int version;                      /**< API version. This version is greater than or equal to \p CNDEV_VERSION_5.*/
  unsigned int subsystemId;         /**< The Sub-System ID of PCIe.*/
  unsigned int deviceId;            /**< The Device ID of PCIe.*/
  __uint16_t vendor;                /**< The Vendor ID of PCIe.*/
  __uint16_t subsystemVendor;       /**< The Sub-System Vendor ID of PCIe.*/
  unsigned int domain;              /**< The Domain number of PCIe.*/
  unsigned int bus;                 /**< The bus number of PCIe.*/
  unsigned int device;              /**< The device number of PCIe.*/
  unsigned int function;            /**< The function number of PCIe.*/
  const char * physicalSlot;        /**< The value of /sys/bus/pci/slots/slotID/address.*/
  int slotID;                       /**< The OAM mainboard slot number obtained out-of-band. This information is valid only on OAM Card products. It is invalid when on PCIe Card products.*/
} cndevPCIeInfo_t;

/** PCie throughput information.*/
typedef struct {
  int version;                /**< API version.*/
  __int64_t pcieRead;         /**< The throughput of reading data in the past 20ms, unit: Byte.*/
  __int64_t pcieWrite;        /**< The throughput of writing data in the past 20ms, unit: Byte.*/
} cndevPCIethroughput_t;

/** PCie throughput information.*/
typedef struct {
  __int64_t pcieRead;         /**< The throughput of reading data in the past 20ms, unit: Byte.*/
  __int64_t pcieWrite;        /**< The throughput of writing data in the past 20ms, unit: Byte.*/
  __int64_t pcieReadTotal;    /**< The total throughput of reading data since load driver, unit: Byte.*/
  __int64_t pcieWriteTotal;   /**< The total throughput of writing data since load driver, unit: Byte.*/
  __uint64_t timestamp;       /**< The timestamp of throughput since 1970(in Microseconds).*/
  __int64_t reserved[8];      /**< Reserved.*/
} cndevPCIethroughputV2_t;

/** Device affinity information.*/
typedef struct {
  int version;          /**< API version.*/
  __uint32_t cpuCount;  /**< The CPU count.*/
  __uint32_t cpuAffinityBitMap[1024]; /**< If there are 80 CPUs in the system, bitmap's format is:
                                       *bitmap[0]:31-16:not used, 15-0:cpu79-cpu64
                                       *bitmap[1]:31-0:cpu63-cpu32
                                       *bitmap[2]:31-0:cpu31-cpu0.
                                     .*/
} cndevAffinity_t;

/** Topology relationship information.*/
typedef struct {
  int version;                              /**< API version.*/
  cndevTopologyRelationshipEnum_t relation; /**< The topology relation.*/
} cndevTopologyRelationship_t;

/** Device current PCI information.*/
typedef struct {
  int version;      /**< API version.*/
  int currentSpeed; /**< PCIe generation.*/
  int currentWidth; /**< PCIe link width.*/
} cndevCurrentPCIInfo_t;

/** Device Max PCI information.**/
typedef struct {
  int version;     /**< API version.*/
  int maxSpeed;    /**< PCIe generation.*/
  int maxWidth;    /**< PCIe link width.*/
}cndevPCIInfo_t;

/** Topology node capability information.*/
typedef struct cndevTopologyNodeCapInfo_t {
  struct cndevTopologyNodeCapInfo_t *next;  /**< The pointer of the next node.*/
  __uint8_t id;                             /**< The capability ID of the current node.*/
  __uint16_t cap;                           /**< The capability value of the current node.*/
} cndevTopologyNodeCapInfo_t;

/** Topology node information.*/
typedef struct cndevTopologyNode_t {
  int virtualRootNode;  
  /**
   * The value is set to 1 when it is the virtual root node, and set to 0 when it is the real node.
   */
  int domain;               /**< Domain in PCIe.*/
  int bus;                  /**< Bus number in PCIe.*/
  int device;               /**< Device in PCIe.*/
  int function;             /**< Function in PCIe.*/
  unsigned int subsystemId; /**< Subsystem ID.*/
  unsigned int deviceId;    /**< Device ID of PCIe.*/ 
  unsigned int vendor;      /**< Vendor ID of PCIe.*/ 
  unsigned int subsystemVendor;  /**< Subsystem vendor ID of PCIe.*/
  char const *deviceName;        /**< Device name.*/
  unsigned int classVal;         /**< Value of the class register of PCIe.*/
  char const *className;         /**< Class name.*/
  struct cndevTopologyNodeCapInfo_t *firstCap; /**< First capability.*/
  struct cndevTopologyNode_t *parent;          /**< Parent node of this node.*/
  struct cndevTopologyNode_t *left;            /**< Left node of this node.*/
  struct cndevTopologyNode_t *right;           /**< Right node of this node.*/
  struct cndevTopologyNode_t *child;           /**< Child node of this node.*/
  unsigned int linkSpeed;                      /**< Link Speed of the device.*/
  int isBridge;                 /**< The value is set to true when the component is a bridge, and set to false when it is not a bridge.*/
  int isCardbus;                /**< The value is set to true when the component is a CardBus, and set to false when it is not a CardBus.*/
  unsigned char primaryBus;     /**< Primary Bus.*/
  unsigned char secondaryBus;   /**< Secondary Bus.*/
  unsigned char subordinateBus; /**< Subordinate Bus.*/
  int acsCtrl;                  /**<ACS control information.*/
} cndevTopologyNode_t;

/** Topology node capability information.*/
typedef struct {
  int version;      /**< API version */
  __uint8_t id;     /**< Capability ID */
  __uint16_t cap;   /**< Capability value */
} cndevCapabilityInfo_t;

/** Health status of device.*/
typedef enum {
  CNDEV_HEALTH_STATE_DEVICE_IN_PROBLEM = 0, /**< The device failure.*/
  CNDEV_HEALTH_STATE_DEVICE_GOOD = 1,       /**< The device in good condition.*/
} cndevHealthStateEnum_t;

/** MLU Driver health types.*/
typedef enum {
  CNDEV_HEALTH_STATE_DRIVER_EARLY_INITED = 0, /**< The driver is in the early stages of its initialization process.*/
  CNDEV_HEALTH_STATE_DRIVER_BRING_UP = 1,     /**< The driver is loading.*/
  CNDEV_HEALTH_STATE_DRIVER_BOOTING = 2,      /**< The driver is booting.*/
  CNDEV_HEALTH_STATE_DRIVER_LATEINIT = 3,     /**< The driver is in the later stages of its initialization process.*/
  CNDEV_HEALTH_STATE_DRIVER_RUNNING = 4,      /**< The driver is running normally without any issue.*/
  CNDEV_HEALTH_STATE_DRIVER_BOOT_ERROR = 5,   /**< An error occurred during the booting process.*/
  CNDEV_HEALTH_STATE_DRIVER_RESET = 6,        /**< The driver is resetting.*/
  CNDEV_HEALTH_STATE_DRIVER_RESET_ERROR = 7,  /**< An error occurred during the resetting process.*/
  CNDEV_HEALTH_STATE_DRIVER_UNKNOWN = 8,      /**< An unknown error occurred.*/
} cndevDriverHealthStateEnum_t;

/** Device and MLU Driver health information.*/
typedef struct {
  int version;                                 /**< API version.*/
  int health;                                  /**< The health flag.*/
  cndevHealthStateEnum_t deviceState;          /**< The health status of device.*/
  cndevDriverHealthStateEnum_t driverState;    /**< The health status of MLU Driver.*/
} cndevCardHealthState_t;

/** Health diagnosis error codes of the incidents.*/
typedef enum {
  CNDEV_FR_OK,                              /**< No errors.*/
  CNDEV_FR_UNKNOWN,                         /**< Unknown error code.*/
  CNDEV_FR_INTERNAL,                        /**< An internal error was detected.*/
  CNDEV_FR_XID_ERROR,                       /**< An Xid error detected.*/
  CNDEV_FR_PCIE_REPLAY_RATE,                /**< Unacceptable rate of PCI errors.*/
  CNDEV_FR_PCIE_GENERATION,                 /**< PCIe generation is too low.*/
  CNDEV_FR_PCIE_WIDTH,                      /**< PCIe width is too low.*/
  CNDEV_FR_LOW_BANDWIDTH,                   /**< The bandwidth is unacceptably low.*/
  CNDEV_FR_HIGH_LATENCY,                    /**< PCIe latency is too high.*/
  CNDEV_FR_MLULINK_DOWN,                    /**< An MLU-Link is down.*/
  CNDEV_FR_MLULINK_REPLAY,                  /**< MLU-Link replay errors detected.*/
  CNDEV_FR_MLULINK_ERROR_THRESHOLD,         /**< Unacceptable rate of MLU-Link errors.*/
  CNDEV_FR_MLULINK_CRC_ERROR_THRESHOLD,     /**< Too many CRC errors happened in MLU-Link system.*/
  CNDEV_FR_DIE2DIE_CRC_ERROR_THRESHOLD,     /**< Too many CRC errors happened in Die-to-Die system.*/
  CNDEV_FR_SBE_VIOLATION,                   /**< Single bit error detected.*/
  CNDEV_FR_DBE_VIOLATION,                   /**< Double bit error detected.*/
  CNDEV_FR_SBE_THRESHOLD_VIOLATION,         /**< SBE threshold violated.*/
  CNDEV_FR_DBE_THRESHOLD_VIOLATION,         /**< DBE threshold violated.*/
  CNDEV_FR_ROW_REMAP_FAILURE,               /**< Row remapping failed.*/
  CNDEV_FR_PENDING_ROW_REMAP,               /**< Row remapping is pending.*/
  CNDEV_FR_ADDRESSSWAP_FAILURE,             /**< Address swap failed.*/
  CNDEV_FR_PENDING_ADDRESSSWAP,             /**< Address swap is pending.*/
  CNDEV_FR_REPAIR_FAILURE,                  /**< Page repair failed.*/
  CNDEV_FR_PENDING_PAGE_REPAIR,             /**< Pending page repair detected.*/
  CNDEV_FR_PENDING_PAGE_RETIREMENTS,        /**< Pending page retirements detected.*/
  CNDEV_FR_PAGE_REPAIR_RESOURCE_THRESHOLD,  /**< Unacceptable total page retirements detected.*/
  CNDEV_FR_TEMP_VIOLATION,                  /**< Temperature is too high.*/
  CNDEV_FR_THERMAL_VIOLATION,               /**< Thermal violation detected.*/
  CNDEV_FR_DEVICE_COUNT_MISMATCH,           /**< Disagreement in MLU count between /dev and CNDev.*/
  CNDEV_FR_DRIVER_ERROR,                    /**< Driver error detected.*/
  CNDEV_FR_DEVSYS_ERROR,                    /**< Device system error detected.*/
} cndevHealthError_t;

/** Health check results.*/
typedef enum {
  CNDEV_HEALTH_RESULT_PASS = 0,          /**< All results within this system are normal.*/
  CNDEV_HEALTH_RESULT_WARN = 10,         /**< A warning has been issued.*/
  CNDEV_HEALTH_RESULT_FAIL = 20,         /**< A failure has been issued.*/
} cndevHealthResult_t;

/** Health check system.*/
typedef enum {
  CNDEV_HEALTH_SYSTEM_PCIE       = 0x1,   /**< PCIe system.*/
  CNDEV_HEALTH_SYSTEM_NCS        = 0x2,   /**< MLU-Link system.*/
  CNDEV_HEALTH_SYSTEM_PMU        = 0x4,   /**< Power management unit.*/
  CNDEV_HEALTH_SYSTEM_MCU        = 0x8,   /**< Micro-controller unit.*/
  CNDEV_HEALTH_SYSTEM_MEM        = 0x10,  /**< Memory.*/
  CNDEV_HEALTH_SYSTEM_THERMAL    = 0x20,  /**< Thermal.*/
  CNDEV_HEALTH_SYSTEM_POWER      = 0x40,  /**< Power.*/
  CNDEV_HEALTH_SYSTEM_DRIVER     = 0x80,  /**< Driver-related.*/
} cndevHealthSystem_t;

#define CNDEV_ERR_MSG_LENGTH 512

/** Health diagnosis information about the error.*/
typedef struct {
  char msg[CNDEV_ERR_MSG_LENGTH];      /**< Messages for the error codes.*/
  __uint32_t device_id;                /**< Device ID of the incidents.*/
  __uint32_t bdf;                      /**< Device BDF of the incidents.*/
  cndevHealthError_t code;             /**< Error codes of the incidents.*/
  __uint32_t category;                 /**< Reserved.*/
  __uint32_t severity;                 /**< Reserved.*/
} cndevDiagErrorDetail_t;

/** Health diagnosis incident information.*/
typedef struct {
  cndevHealthSystem_t system;          /**< System to which this information belongs.*/
  cndevHealthResult_t health;          /**< Health diagnosis of this incident.*/
  cndevDiagErrorDetail_t error;        /**< Information about this error and its error code.*/
} cndevIncidentInfo_t;

#define CNDEV_HEALTH_SYSTEM_MAX_INCIDENTS 64

/** Device and MLU Driver health information, which includes the health incidents of each system.*/
typedef struct {
  int health;                                                        /**< The health flag.*/
  cndevHealthStateEnum_t deviceState;                                /**< The health status of device.*/
  cndevDriverHealthStateEnum_t driverState;                          /**< The health status of MLU Driver.*/
  cndevHealthResult_t overallHealth;                                 /**< The overall health status.*/
  __uint32_t incident_count;                                         /**< The number of health incidents reported.*/
  cndevIncidentInfo_t incidents[CNDEV_HEALTH_SYSTEM_MAX_INCIDENTS];  /**< Report of the errors detected.*/
  __uint32_t reserved[8];                                            /**< Reserved.*/
} cndevCardHealthStateV2_t;

/** Heartbeat information.*/
typedef struct {
  int version;                /**< API version.*/
  __uint32_t heartbeatCount;  /**< The count of heartbeats.*/
} cndevCardHeartbeatCount_t;

/** Device Link speed.*/
typedef struct {
    int version;     /**< API version.*/
    int linkSpeed;   /**< The speed, 1-2.5GT/s;2-5GT/s;3-8GT/s;4-16GT/s.*/
} cndevLinkSpeed_t;  

/** Device VPU utilization.*/
typedef struct {
    int version;                /**< API version.*/
    int vpuCount;               /**< The count of VPUs.*/
    int vpuCodecUtilization[20];/**< The utilization of each VPU, the valid array index range [0, ``vpuCount`` - 1].*/
    int encoderCount;           /**< The count of video encoders.*/
    int encoderUtilization[20]; /**< The utilization of each encoder, the valid array index range [0, ``encoderCount`` - 1].*/
    int decoderCount;           /**< The count of video decoders.*/
    int decoderUtilization[20]; /**< The utilization of each decoder, the valid array index range [0, ``decoderCount`` - 1].*/
}cndevVideoCodecUtilization_t;

/** Device JPU utilization.*/
typedef struct {
    int version;                /**< API version.*/
    int jpuCount;               /**< The count of JPUs.*/
    int jpuCodecUtilization[20];/**< The utilization of each JPU, the valid array index range [0, ``jpuCount`` - 1].*/
}cndevImageCodecUtilization_t;

/** Device Fast memory allocation.*/
typedef struct {
  int version;          /**< API version.*/
  int fastMemoryTotal;  /**< The total size of fast memory in KByte.*/
  int fastMemoryFree;   /**< The size of free fast memory available for allocation in KByte.*/
}cndevFastAlloc_t;

/** NUMA node ID.*/
typedef struct {
  int version;      /**< API version.*/
  __int32_t nodeId; /**< NUMA Node ID.*/
} cndevNUMANodeId_t;

/** Device Scaler utilization.*/
typedef struct {
    int version;                /**< API version.*/
    int scalerCount;            /**< The count of scalers.*/
    int scalerUtilization[20];  /**< The utilization of each scaler, the valid array index range [0, ``scalerCount`` - 1].*/
}cndevScalerUtilization_t;

/** Codec Turbo information.*/
typedef struct {
  int version;        /**< API version.*/
  int codecTurbo;     /**< Codec Turbo status. \p 0 represents  \p DISABLE; \p 1 represents \p ENABLE.*/
}cndevCodecTurbo_t;

/** Device memory information.*/
typedef struct {
  int version; /**< API version.*/
  int count;   /**< The memory die count of devices.*/
} cndevCardMemoryDieCount_t;

/** QSFP interface status, only valid on MLU290.*/
typedef struct {
  int version; /**< API version.*/
  int qdd[8];  /**< QSFP interface status of each MLU-Link port.*/
} cndevQsfpddStatus_t;

/** MLU-Link version.*/
typedef struct {
  int version;              /**< API version.*/
  unsigned majorVersion;    /**< The major version number.*/
  unsigned minorVersion;    /**< The minor version number.*/
  unsigned buildVersion;    /**< The patch version number.*/
} cndevMLULinkVersion_t;

/** MLU-Link SerDes status.*/
typedef enum {
  CNDEV_MLULINK_STATE_DISABLE = 0,  /**< The status of MLU-Link is disabled.*/
  CNDEV_MLULINK_STATE_ENABLE = 1,   /**< The status of MLU-Link is enabled.*/
  CNDEV_MLULINK_STATE_ONGOING = 2,  /**< The status of MLU-Link is that the link is loading.*/
  CNDEV_MLULINK_STATE_COUNT,        /**< The count of MLU-Link statuses.*/
} cndevMLULinkStateEnum_t;

/** MLU-Link cable status.*/
typedef enum {
  CNDEV_MLULINK_CABLE_STATE_UNCONNECTED = 0,    /**< The MLU-Link cable is not connected.*/
  CNDEV_MLULINK_CABLE_STATE_CONNECTED = 1,      /**< The MLU-Link cable is connected.*/
  CNDEV_MLULINK_CABLE_STATE_UNIMPLEMENTED = 2,  /**< The MLU-Link cable connection is not supported.*/
  CNDEV_MLULINK_CABLE_STATE_COUNT,              /**< The count of MLU-Link cable connections.*/
} cndevMLULinkCableStateEnum_t;

/** MLU-Link status.*/
typedef struct {
  int version;                              /**< API version.*/
  cndevEnableStatusEnum_t isActive;         /**< The status of the MLU-Link port.*/
  cndevMLULinkStateEnum_t serdesState;      /**< The SerDes status of the MLU-Link port.*/
  cndevMLULinkCableStateEnum_t cableState;  /**< The cable status of the MLU-Link port.*/
} cndevMLULinkStatus_t;

/** MLU-Link MAC status.*/
typedef enum {
  CNDEV_MLULINK_MAC_STATE_DOWN = 0,   /**< Link down.*/
  CNDEV_MLULINK_MAC_STATE_UP = 1,     /**< Link up.*/
  CNDEV_MLULINK_MAC_STATE_LLDP = 2,   /**< LLDP packet received.*/
  CNDEV_MLULINK_MAC_STATE_READY = 3,  /**< Link ready with a configured IP address.*/
} cndevMLULinkMACStateEnum_t;

/** MLU-Link Serializer/Deserializer status.*/
typedef enum {
  CNDEV_MLULINK_SERDES_STATE_DISABLE = 0,  /**< Failed to establish link.*/
  CNDEV_MLULINK_SERDES_STATE_ENABLE = 1,   /**< Established link.*/
  CNDEV_MLULINK_SERDES_STATE_ONGOING = 2,  /**< Establishing link.*/
} cndevMLULinkSerdesStateEnum_t;

/** MLU-Link Presence status.*/
typedef enum {
  CNDEV_MLULINK_PRESENCE_STATE_ABSENT = 0,         /**< The optical module or cable is absent.*/
  CNDEV_MLULINK_PRESENCE_STATE_PRESENT = 1,        /**< The optical module or cable is present.*/
  CNDEV_MLULINK_PRESENCE_STATE_UNIMPLEMENTED = 2,  /**< The optical module or cable is not supported.*/
} cndevMLULinkPresenceStateEnum_t;

/** MLU-Link status information.*/
typedef struct {
  cndevMLULinkMACStateEnum_t macState;            /**< The MAC status of the MLU-Link.*/
  cndevMLULinkSerdesStateEnum_t serdesState;      /**< The SerDes status of the MLU-Link.*/
  cndevMLULinkPresenceStateEnum_t presenceState;  /**< The Presence status of the MLU-Link.*/
  __uint32_t reserved[8];                         /**< Reserved.*/
} cndevMLULinkStatusV2_t;

/** MLU-Link optical status for temperature, voltage, TX/RX power and TX bias current.*/
typedef enum {
  CNDEV_MLULINK_OPTICAL_STATE_NORMAL = 0,    /**< Normal status.*/
  CNDEV_MLULINK_OPTICAL_STATE_HIGHALARM = 1, /**< High alarm status.*/
  CNDEV_MLULINK_OPTICAL_STATE_LOWALARM = 2,  /**< Low alarm status.*/
  CNDEV_MLULINK_OPTICAL_STATE_HIGHWARN = 3,  /**< High warning status.*/
  CNDEV_MLULINK_OPTICAL_STATE_LOWWARN = 4,   /**< Low warning status.*/
} cndevOpticalStatusEnum_t;

/** Optical monitor thresholds.*/
typedef struct {
  float high_alarm; /**< High alarm threshold.*/
  float low_alarm;  /**< Low  alarm threshold.*/
  float high_warn;  /**< High warning threshold.*/
  float low_warn;   /**< Low  warning threshold.*/
} cndevOpticalThres_t;

/** Optical information.*/
typedef struct {
  __uint8_t qdd_idx;                       /**< The scale out port index.*/
  __uint8_t split_idx;                     /**< The splitting index of the optical module.*/
  __uint8_t present;                       /**< Installation status of the optical module.*/
  char vendor_name[16];                    /**< Vendor name.*/
  char vendor_pn[16];                      /**< Vendor device ID.*/
  char vendor_sn[16];                      /**< Vendor serial number.*/
  __uint8_t vendor_oui[3];                 /**< Unique identifier of the vendor organization.*/
  __uint8_t identifier;                    /**< Identity information of the optical module.*/
  float wave_length;                       /**< Optical module wavelength, unit: nm.*/
  char manu_dc[8];                         /**< Manufacturing date code.*/
  float temp;                              /**< Optical module temperature, degrees Celsius.*/
  cndevOpticalStatusEnum_t temp_state;     /**< The temperature status of optical module.*/
  cndevOpticalThres_t temp_thresh;         /**< Temperature monitor thresholds. */
  float volt;                              /**< Power voltage of the optical module, unit: V.*/
  cndevOpticalStatusEnum_t volt_state;     /**< The voltage status of optical module.*/
  cndevOpticalThres_t volt_thresh;         /**< Supply 3.3-volt monitor thresholds.*/

  int valid_lane_num;                      /**< The valid number of lane information for ``txpwr``, ``txpwr_state``, ``txbias``, ``txbias_state``, ``rxpwr`` and ``rxpwr_state``.*/
  float txpwr[8];                          /**< TX[1:8] output optical power, unit: dBm.*/
  cndevOpticalStatusEnum_t txpwr_state[8]; /**< TX[1:8] optical power status.*/
  cndevOpticalThres_t txpwr_thresh;        /**< TX optical power monitor thresholds.*/
  float txbias[8];                         /**< TX[1:8] bias current, unit: mA.*/
  cndevOpticalStatusEnum_t txbias_state[8];/**< TX[1:8] bias current status.*/
  cndevOpticalThres_t txbias_thresh;       /**< TX bias current monitor thresholds.*/
  float rxpwr[8];                          /**< RX[1:8] input optical power, unit: dBm.*/
  cndevOpticalStatusEnum_t rxpwr_state[8]; /**< RX[1:8] optical power status.*/
  cndevOpticalThres_t rxpwr_thresh;        /**< RX optical power thresholds. */
  __uint8_t txlos_flag;                    /**< TX[1:8] optical power loss flag.*/
  __uint8_t rxlos_flag;                    /**< RX[1:8] optical power loss flag.*/
  __uint64_t reserved[8];                  /**< Reserved.*/
} cndevOpticalInfo_t;

/** MLU-Link speed.*/
typedef struct {
  int version;                          /**< API version.*/
  float speedValue;                     /**< The max speed of MLU-Link port, unit: GB/s.*/
  cndevMLULinkSpeedEnum_t speedFormat;  /**< The format of speed.*/
} cndevMLULinkSpeed_t;

/** MLU-Link capability, only valid on MLU370.*/
typedef struct {
  int version;                /**< API version.*/
  unsigned p2pTransfer;       /**< The capability of p2p Transfer.*/
  unsigned interlakenSerdes;  /**< The capability of interlaken SerDes.*/
} cndevMLULinkCapability_t;

/** @brief @deprecated MLU-Link statistical count.*/
typedef struct {
  int version;                    /**< API version.*/
  __uint64_t cntrReadByte;        /**< The count of read bytes.*/
  __uint64_t cntrReadPackage;     /**< The count of read packages.*/
  __uint64_t cntrWriteByte;       /**< The count of write bytes.*/
  __uint64_t cntrWritePackage;    /**< The count of write packages.*/
  __uint64_t errReplay;           /**< The count of replay packages.*/
  __uint64_t errFatal;            /**< The count of failures.*/
  __uint64_t errEccDouble;        /**< The count of ECC Double errors.*/
  __uint64_t errCRC24;            /**< The count of CRC24 errors.*/
  __uint64_t errCRC32;            /**< The count of CRC32 errors.*/
  __uint64_t errCorrected;        /**< The count of corrected errors.*/
  __uint64_t errUncorrected;      /**< The count of uncorrected errors.*/
  __uint64_t errReadPackage;      /**< The count of failed attempts to read packages.*/
  __uint64_t errWritePackage;     /**< The count of failed attempts to write packages.*/
  __uint64_t errIllegalAccess;    /**< The count of illegal access.*/
  __uint64_t cntrCnpPackage;      /**< The count of CNP packages.*/
  __uint64_t cntrPfcPackage;      /**< The count of PFC packages.*/
  __uint64_t statQPInUsing;       /**< The count of QPs in use.*/
  __uint64_t statTaskAlive;       /**< The count of active tasks.*/
} cndevMLULinkCounter_t;

/** MLU-Link statistics information.*/
typedef struct {
  __uint64_t rxPfcOkPkt;	/**< The total count of received OK PFC packets.*/
  __uint64_t rxPfc0Pkt;		/**< The count of received PFC packets with priority 0.*/
  __uint64_t rxPfc1Pkt;		/**< The count of received PFC packets with priority 1.*/
  __uint64_t rxPfc2Pkt;		/**< The count of received PFC packets with priority 2.*/
  __uint64_t rxPfc3Pkt;		/**< The count of received PFC packets with priority 3.*/
  __uint64_t rxPfc4Pkt;		/**< The count of received PFC packets with priority 4.*/
  __uint64_t rxPfc5Pkt;		/**< The count of received PFC packets with priority 5.*/
  __uint64_t rxPfc6Pkt;		/**< The count of received PFC packets with priority 6.*/
  __uint64_t rxPfc7Pkt;		/**< The count of received PFC packets with priority 7.*/

  __uint64_t txPfcOkPkt;	/**< The total count of sent OK PFC packets.*/
  __uint64_t txPfc0Pkt;		/**< The count of sent PFC packets with priority 0.*/
  __uint64_t txPfc1Pkt;		/**< The count of sent PFC packets with priority 1.*/
  __uint64_t txPfc2Pkt;		/**< The count of sent PFC packets with priority 2.*/
  __uint64_t txPfc3Pkt;		/**< The count of sent PFC packets with priority 3.*/
  __uint64_t txPfc4Pkt;		/**< The count of sent PFC packets with priority 4.*/
  __uint64_t txPfc5Pkt;		/**< The count of sent PFC packets with priority 5.*/
  __uint64_t txPfc6Pkt;		/**< The count of sent PFC packets with priority 6.*/
  __uint64_t txPfc7Pkt;		/**< The count of sent PFC packets with priority 7.*/

  __uint64_t rxRocePkt;		/**< The count of packets received by RoCE.*/
  __uint64_t txRocePkt;		/**< The count of packets sent by RoCE.*/
  __uint64_t roceVerificationErrPkt;/**< The count of error packets verified by RoCE.*/

  __uint64_t reserved[64];             /**< Reserved.*/
} cndevMLULinkCounterExt_t;

/** MLU-Link basic counter information.*/
typedef struct {
  __uint64_t rxByte;		/**< The total number of bytes of all packets received with no error.*/
  __uint64_t rxPkt;		/**< The count of all packets received with no error.*/
  __uint64_t txByte;		/**< The total number of bytes of all packets transmitted with no error.*/
  __uint64_t txPkt;		/**< The count of all packets sent with no error.*/
  __uint64_t rxRocePkt;		/**< The count of all packets received by RoCE with no error.*/
  __uint64_t txRocePkt;		/**< The count of all packets transmitted by RoCE with no error.*/

  __uint64_t reserved[26];	/**< Reserved.*/
} cndevMLULinkBasicCounter_t;

/** MLU-Link congestion control counter information.*/
typedef struct {
  __uint64_t rxCnpPkt;		/**< The count of Congestion Notification Packets received.*/
  __uint64_t reserved[7];	/**< Reserved.*/
} cndevMLULinkCongestionCtrlCounter_t;

/** MLU-Link event counter information.*/
typedef struct {
  __uint64_t correctFecCnt;	/**< The count of corrected codewords by FEC correction algorithms.*/
  __uint64_t uncorrectFecCnt;	/**< The count of uncorrected codewords by FEC correction algorithms.*/
  __uint64_t rxBadFcsPkt;	/**< The total count of packets received with bad FCS.*/
  __uint64_t txBadFcsPkt;	/**< The total count of packets transmitted with bad FCS.*/
  __uint64_t illegalAccessCnt;	/**< The count of illegal memory accesses detected while receiving data.*/
  __uint64_t reserved[11];	/**< Reserved.*/
} cndevMLULinkErrorCounter_t;

/** MLU-Link flow control counter information.*/
typedef struct {
  __uint64_t rxPfcPkt;		/**< The total count of received PFC packets.*/
  __uint64_t rxPfc0Pkt;		/**< The count of received PFC packets with priority 0.*/
  __uint64_t rxPfc1Pkt;		/**< The count of received PFC packets with priority 1.*/
  __uint64_t rxPfc2Pkt;		/**< The count of received PFC packets with priority 2.*/
  __uint64_t rxPfc3Pkt;		/**< The count of received PFC packets with priority 3.*/
  __uint64_t rxPfc4Pkt;		/**< The count of received PFC packets with priority 4.*/
  __uint64_t rxPfc5Pkt;		/**< The count of received PFC packets with priority 5.*/
  __uint64_t rxPfc6Pkt;		/**< The count of received PFC packets with priority 6.*/
  __uint64_t rxPfc7Pkt;		/**< The count of received PFC packets with priority 7.*/

  __uint64_t txPfcPkt;		/**< The total count of sent OK PFC packets.*/
  __uint64_t txPfc0Pkt;		/**< The count of sent PFC packets with priority 0.*/
  __uint64_t txPfc1Pkt;		/**< The count of sent PFC packets with priority 1.*/
  __uint64_t txPfc2Pkt;		/**< The count of sent PFC packets with priority 2.*/
  __uint64_t txPfc3Pkt;		/**< The count of sent PFC packets with priority 3.*/
  __uint64_t txPfc4Pkt;		/**< The count of sent PFC packets with priority 4.*/
  __uint64_t txPfc5Pkt;		/**< The count of sent PFC packets with priority 5.*/
  __uint64_t txPfc6Pkt;		/**< The count of sent PFC packets with priority 6.*/
  __uint64_t txPfc7Pkt;		/**< The count of sent PFC packets with priority 7.*/

  __uint64_t reserved[14];	/**< Reserved.*/
} cndevMLULinkFlowCtrlCounter_t;

/** MLU-Link event counter information.*/
typedef struct {
  __uint64_t linkDown;		/**< The count of MLU-Link disconnections.*/
  __uint64_t replay;		/**< The total count of task replays.*/
  __uint64_t replayFail;	/**< The count of task replay failures.*/
  __uint64_t reserved[13];	/**< Reserved.*/
} cndevMLULinkEventCounter_t;

/** MLU-Link task statistics counter information.*/
typedef struct {
  __uint64_t qpInUsing;       	/**< The count of QPs in use on an MLU-Link.*/
  __uint64_t taskAlive;       	/**< The count of tasks on an MLU-Link.*/
  __uint64_t reserved[14];	/**< Reserved.*/
} cndevMLULinkTaskStatsCounter_t;

/** @brief @deprecated MLU-Link count information.*/
typedef struct {
  int version;                          /**< API version.*/
  cndevMLULinkCounterEnum_t setCounter; /**< The count type.*/
} cndevMLULinkSetCounter_t;

/** MLU-Link remote information.*/
typedef struct {
  int version;                                /**< API version.*/
  __uint64_t mcSn;                            /**< The mezzanine device SN of device that connected with MLU-Link.*/
  __uint64_t baSn;                            /**< The board SN of device that connected with MLU-Link.*/
  __uint32_t slotId;                          /**< The slot Id of device that connected with MLU-Link.*/
  __uint32_t portId;                          /**< The port Id of device that connected with MLU-Link.*/
  __uint8_t devIp[16];                        /**< The ipaddr of device that connected with MLU-Link.*/
  __uint8_t uuid[CNDEV_UUID_SIZE];            /**< The UUID of device that connected with MLU-Link.*/
  __uint32_t devIpVersion;                    /**< The ipaddr version of device that connected with MLU-Link.*/
  __uint32_t isIpValid;                       /**< The ipaddr of device that connected with MLU-Link valid flag.*/
  __int32_t connectType;                      /**< The connect type of device that connected with MLU-Link.*/
  __uint64_t ncsUUID64;                       /**< The ncs UUID of device that connected with MLU-Link.*/
  __uint8_t  mac_addr[CNDEV_MAC_ADDRESS_LEN]; /**< The MAC address of device that connected with MLU-Link.*/
  char port_name[CNDEV_MLU_REMOTE_PORT_NAME_LEN];  /**< The port name of device that connected with MLU-Link.*/
} cndevMLULinkRemoteInfo_t;

/** MLU-Link devices SN.*/
typedef struct {
  int version;
  __uint64_t mlulinkMcSn[8];        /**< The mezzanine device SN of device that connected with MLU-Link.*/
  __uint64_t mlulinkBaSn[8];        /**< The board SN of device that connected with MLU-Link.*/
} cndevMLULinkDevSN_t;

/** NVME information.*/
typedef struct {
  __uint8_t nvmeSn[21];             /**< The NVME SSD SN.*/
  __uint8_t nvmeModel[17];          /**< The NVME SSD model.*/
  __uint8_t nvmeFw[9];              /**< The NVME SSD firmware version.*/
  __uint8_t nvmeMfc[9];             /**< The NVME SSD manufacturer.*/
} cndevNvmeSsdInfo_t;

/** PSU information.*/
typedef struct {
  __uint8_t psuSn[17];              /**< The PSU SN.*/
  __uint8_t psuModel[17];           /**< The PSU model.*/
  __uint8_t psuFw[17];              /**< The PSU firmware version.*/
  __uint8_t psuMfc[17];             /**< The PSU manufacturer.*/
} cndevPsuInfo_t;

/** IB information.*/
typedef struct {
  __uint8_t ibSn[25];               /**< The IB SN.*/
  __uint8_t ibModel[17];            /**< The IB model.*/
  __uint8_t ibFw[5];                /**< The IB firmware version.*/
  __uint8_t ibMfc[9];               /**< The IB manufacturer.*/
} cndevIbInfo_t;

/** Chassis information.*/
typedef struct {
  int version;                      /**< API version.*/
  __uint64_t chassisSn;             /**< Chassis SN.*/
  char chassisProductDate[12];      /**< Chassis product date.*/
  char chassisPartNum[13];          /**< Chassis production model.*/
  char chassisVendorName[17];       /**< Chassis manufacturer.*/
  __uint8_t nvmeSsdNum;             /**< The count of NVME SSDs.*/
  cndevNvmeSsdInfo_t nvmeInfo[8];   /**< The information of NVME SSD.*/
  __uint8_t ibBoardNum;             /**< The count of IBs.*/
  cndevIbInfo_t ibInfo[8];          /**< The information of IB.*/
  __uint8_t psuNum;                 /**< The count of PSUs.*/
  cndevPsuInfo_t psuInfo[6];        /**< The information of PSU.*/
  __uint8_t bmcIPVersion;           /**< The Falcon chassis BMC IP address version.*/
  __uint8_t bmcIP[CNDEV_IP_ADDRESS_LEN]; /**< The Falcon chassis BMC IP address.*/
} cndevChassisInfo_t;

/** Chassis information V2.*/
typedef struct {
  int version;                            /**< API version.*/
  __uint64_t chassisSn;                   /**< Chassis SN.*/
  char chassisProductDate[12];            /**< Chassis product date.*/
  char chassisProductName[13];            /**< Chassis product name.*/
  char chassisVendorName[17];             /**< Chassis manufacturer.*/
  __uint8_t nvmeSsdNum;                   /**< The count of NVME SSDs.*/
  cndevNvmeSsdInfo_t nvmeInfo[8];         /**< The information of NVME SSD.*/
  __uint8_t ibBoardNum;                   /**< The count of IBs.*/
  cndevIbInfo_t ibInfo[8];                /**< The information of IB.*/
  __uint8_t psuNum;                       /**< The count of PSUs.*/
  cndevPsuInfo_t psuInfo[6];              /**< The information of PSU.*/
  __uint8_t bmcIPVersion;                 /**< The Falcon chassis BMC IP address version.*/
  __uint8_t bmcIP[CNDEV_IP_ADDRESS_LEN];  /**< The Falcon chassis BMC IP address.*/
  char chassisPartNumber[32];             /**< Chassis part number.*/
  __uint64_t reserved[16];                /**< Reserved.*/
} cndevChassisInfoV2_t;

/** PCIe firmware information.*/
typedef struct {
  int version;                      /**< API version.*/
  __uint16_t pcieReversion;         /**< PCIe firmware reversion.*/
  __uint16_t pcieBuildID;           /**< PCIe firmware build ID.*/
  __uint16_t pcieEngineeringId;     /**< PCIe firmware engineering ID.*/
} cndevPCIeFirmwareVersion_t;

/** Device CPU utilization.*/
typedef struct {
  int version;                      /**< API version.*/
  __uint16_t chipUtilization;       /**< The average utilization of CPU.*/
  __uint8_t coreNumber;             /**< The count of CPU cores.*/
  __uint8_t coreUtilization[80];    /**< The utilization of each CPU core, the valid array index range [0, ``coreNumber`` - 1].*/
} cndevDeviceCPUUtilization_t;

typedef enum {
  CNDEV_CPUSTATE_USER,           /**< The percentage of CPU occupied by user space.*/
  CNDEV_CPUSTATE_NICE,           /**< The percentage of CPU occupied by processes that have changed their priorities.*/
  CNDEV_CPUSTATE_SYSTEM,         /**< The percentage of CPU occupied by kernel space.*/
  CNDEV_CPUSTATE_SOFTIRQ,        /**< The percentage of CPU occupied by software interrupt.*/
  CNDEV_CPUSTATE_IRQ,            /**< The percentage of CPU occupied by hardware interrupt.*/
  CNDEV_CPUSTATE_IDLE,           /**< The percentage of CPU that is idle.*/
  CNDEV_CPUSTATE_IOWAIT,         /**< The percentage of CPU occupied by IO waiting.*/
  CNDEV_CPUSTATE_MAX = 64,       /**< The max type of CPU state.*/
} cndevDeviceCpuStatUtilEnum_t;

/** Device CPU utilization V2.*/
typedef struct {
  int version;                                                          /**< API version.*/
  __uint16_t chipUtilization;                                           /**< The average utilization of CPU.*/
  __uint8_t coreNumber;                                                 /**< The count of CPU cores.*/
  __uint8_t coreUtilization[CNDEV_MAX_CORE_COUNT];                            /**< The utilization of each CPU core, the valid array index range [0, ``coreNumber`` - 1].*/
  __uint8_t coreStateUtilization[CNDEV_MAX_CORE_COUNT][CNDEV_CPUSTATE_MAX];   /**< The detailed state percentage for each CPU core.*/
} cndevDeviceCPUUtilizationV2_t;

/** Device CPU sampling interval.*/
typedef struct {
  int version;                      /**< API version.*/
  __uint32_t samplingInterval;      /**< The sampling interval for CPU utilization, unit: ms.*/
} cndevDeviceCPUSamplingInterval_t;

/** Reasons of retired page.*/
typedef enum {
  CNDEV_PAGE_RETIREMENT_CAUSE_MULTIPLE_SINGLE_BIT_ECC_ERRORS = 0, /**< Page retirement due to multiple single-bit ECC error.*/
  CNDEV_PAGE_RETIREMENT_CAUSE_DOUBLE_BIT_ECC_ERROR = 1            /**< Page retirement due to double bit ECC error.*/
} cndevRetirePageCauseEnum_t;

/** Retired page information.*/
typedef struct {
  int version;                        /**< API version.*/
  cndevRetirePageCauseEnum_t cause;   /**< The cause is input parameter, represents the reason of retired page.*/
  __uint32_t pageCount;               /**< The number of retired pages matching specific cause.*/
  __uint64_t pageAddress[512];        /**< The list of physical addresses for the retired pages.*/
} cndevRetiredPageInfo_t;

/** Retire page status.*/
typedef struct {
  int version;                /**< API version.*/
  __uint32_t isPending;       /**< Flag indicating if the page retire operation is pending. True if a page retire process is pending. False otherwise. */
  __uint32_t isFailure;       /**< Flag indicating if the page retire operation failed. True if an error occurred during the page retire process. False otherwise. */
} cndevRetiredPageStatus_t;

/** X-axis labels of repair resource histogram. 
 * This value varies in different structures: In @p cndevAddressSwap_t, it represents the number of
 * channels in different repair resource usage situations, and in @p cndevRemappedRowV2_t, it represents
 * the number of banks in different repair resource usage situations.
 **/
typedef enum {
    CNDEV_AVAILABILITY_XLABLE_NONE,  /**< Indicates the number of banks/channels where repair resources are used up. */
    CNDEV_AVAILABILITY_XLABLE_PARTIAL, /**< Indicates the number of banks/channels where repair resources are partially used. */
    CNDEV_AVAILABILITY_XLABLE_MAX, /**< Indicates the number of banks/channels where repair resources are unused. */
    CNDEV_AVAILABILITY_XLABLE_ALL, /**< The number of Xlable members. */
} cndevRepairHistogramXlableEnum_t;

/** Remaped row information.*/
typedef struct {
  int version;                /**< API version.*/
  __uint32_t correctRows;     /**< The count of row remapping resources used by repair correctable memory errors. */
  __uint32_t uncorrectRows;   /**< The count of row remapping resources used by repair uncorrectable memory errors. */
  __uint32_t pendingRows;     /**< Reserved.*/
  __uint32_t failedRows;      /**< Reserved.*/
} cndevRemappedRow_t;

/** The remapped row information. */
typedef struct {
  int version;                /**< API version.*/
  __uint32_t correctCounts;     /**< The count of memory error repair resources used by repair correctable memory errors. */
  __uint32_t uncorrectCounts;   /**< The count of memory error repair resources used by repair uncorrectable memory errors. */
  __uint32_t histogram[CNDEV_AVAILABILITY_XLABLE_ALL]; /**< The histogram value of memory error repair resources. */
} cndevRemappedRowV2_t;

/** Memory error repair status. */
typedef struct {
  int version;            /**< API version.*/
  bool isRetirePending;   /**< Flag indicating if the memory error retire operation is pending. True if a retire process is pending. False otherwise. */
  bool isPending;         /**< Flag indicating if the memory error repair operation is pending. True if a repair process is pending. False otherwise. */
  bool isFailure;         /**< Flag indicating if the memory error repair operation failed. True if an error occurred during the repair process. False otherwise. */
} cndevRepairStatus_t;

/** Address swap information. */
typedef cndevRemappedRowV2_t cndevAddressSwap_t;

/** Retire page switch.*/
typedef struct {
  int version;                                /**< API version.*/
  cndevEnableStatusEnum_t retirePageOption;   /**< The enable of retire page.*/
} cndevRetiredPageOperation_t;

/** VF status.*/
typedef struct {
  int version;        /**< API version.*/
  int vfState;        /**< Virtualization state bitmap.*/
} cndevCardVfState_t;

/** MLU-Link types.*/
typedef enum {
  PORT_WORK_MODE_UNINITIALIZED = 0, /**< Port is uninitialized. */
  PORT_WORK_MODE_ALL_SUPPORT = 1, /**< Port supports all modes. */
  PORT_WORK_MODE_MLULINK = 2, /**< Port supports MLU-Link mode. */
  PORT_WORK_MODE_MLULINK_OVER_ROCE = 3, /**< Port supports MLU-Link over RoCE mode. */
} cndevPortModeEnum_t;

/** MLU-Link port mode information.*/
typedef struct {
  int version;                      /**< API version.*/
  cndevPortModeEnum_t mode;         /**< The current work mode of MLU-Link port.*/
  cndevPortModeEnum_t supportMode;  /**< Reserved.*/
} cndevMLULinkPortMode_t;

/** MLU-Link RoCE field types.*/
typedef enum {
  CNDEV_MLULINK_FIELD_IP_VERSION = 0,   /**< IP version. Value 4 represents IPv4, and 6 represents IPv6.*/
  CNDEV_MLULINK_FIELD_VLAN_TPID,        /**< VLAN TPID (Tag Protocol Identifier).*/
  CNDEV_MLULINK_FIELD_VLAN_CFI,         /**< VLAN CFI (Canonical Format Indicator).*/
  CNDEV_MLULINK_FIELD_VLAN_VID,         /**< VLAN VID.*/
  CNDEV_MLULINK_FIELD_VLAN_EN,          /**< The enabling state for VLAN.*/
  CNDEV_MLULINK_FIELD_IP_TTL,           /**< IP TTL. Only supports IPv4.*/
  CNDEV_MLULINK_FIELD_FLOW_LABLE,       /**< Flow label. Only supports IPv6.*/
  CNDEV_MLULINK_FIELD_HOP_LIMIT,        /**< Hop limit. Only supports IPv6.*/
  CNDEV_MLULINK_FIELD_PFC_XON_ENABLE,   /**< The enabling state for PFC XON.*/ 
  CNDEV_MLULINK_FIELD_PFC_XOFF_ENABLE,  /**< The enabling state for PFC XOFF.*/
  CNDEV_MLULINK_FIELD_PFC_XON_PERIOD,   /**< The pause time for PFC XON.*/
  CNDEV_MLULINK_FIELD_PFC_XOFF_PERIOD,  /**< The pause time for PFC XOFF.*/
  CNDEV_MLULINK_FIELD_PFC_PERIOD,       /**< The pause time for PFC.*/
  CNDEV_MLULINK_FIELD_PFC_EN,           /**< The enabling state for PFC.*/
  CNDEV_MLULINK_FIELD_QOS_TRUST,        /**< The trust mode for QoS. Value 0 represents dot1p, and 1 represents DSCP.*/ 
  CNDEV_MLULINK_FIELD_VLAN_DOT1P,       /**< The mapping between a VLAN and priority.*/
  CNDEV_MLULINK_FIELD_DATA_DOT1P,       /**< The mapping between VLAN data and priority.*/
  CNDEV_MLULINK_FIELD_CTRL_DOT1P,       /**< The mapping between VLAN control and priority.*/
  CNDEV_MLULINK_FIELD_RESP_DOT1P,       /**< The mapping between VLAN RESP and priority.*/
  CNDEV_MLULINK_FIELD_TCP_DSCP,         /**< The mapping between DSCP TCP and priority.*/
  CNDEV_MLULINK_FIELD_DATA_DSCP,        /**< The mapping between DSCP data and priority.*/
  CNDEV_MLULINK_FIELD_CTRL_DSCP,        /**< The mapping between DSCP control and priority.*/
  CNDEV_MLULINK_FIELD_RESP_DSCP,        /**< The mapping between DSCP RESP and priority.*/
  CNDEV_MLULINK_FIELD_NUM,              /**< The total number of MLU-Link RoCE field types.*/
}  cndevMLULinkOverRoCEFieldEnum_t;

/** MLU-Link RoCE ctrl information.*/
typedef struct {
  int version;                            /**< API version.*/
  cndevMLULinkOverRoCEFieldEnum_t field;  /**< The field name of MLU-Link over RoCE port.*/
  __uint32_t value;                       /**< The value of field name.*/
} cndevMLULinkOverRoCECtrl_t;

/** TinyCore utilization.*/
typedef struct {
  int version;                                /**< API version.*/
  int tinyCoreCount;                          /**< The count of tiny cores.*/
  int tinyCoreUtilization[CNDEV_TINYCOREMAXCOUNT];  /**< The utilization of each tiny core, the valid array index range [0, ``tinyCoreCount`` - 1].*/
} cndevTinyCoreUtilization_t;

/** Arm OS memory information.*/
typedef struct {
  int version;                            /**< API version.*/
  union {
    __int64_t armOsMemoryTotal;           /**< Arm system total memory, unit: KB.*/
    __int64_t deviceSystemMemoryTotal;   /**< Device system total memory, unit: KB.*/
  };
  union {
    __int64_t armOsMemoryUsed;          /**< Arm system used memory, unit: KB.*/
    __int64_t deviceSystemMemoryUsed;  /**< Device system used memory, unit: KB.*/
  };
} cndevArmOsMemoryInfo_t;

/** Device OS memory information.*/
typedef struct {
  __uint32_t deviceSystemMemoryTotal;   /**< Device system total memory, unit: KB.*/
  __uint32_t deviceSystemMemoryUsed;    /**< Device system used memory, unit: KB.*/
  __uint32_t reserved[8];               /**< Reserved.*/
} cndevDeviceOsMemoryInfo_t;

/** Chip ID information.*/
typedef struct {
  int version;           /**< API version.*/
  __uint8_t chipId;      /**< Chip ID.*/
} cndevChipId_t;

/** Frequency status.*/
typedef struct {
  int version;                     /**< API version.*/
  __uint8_t mluFrequencyLockStatus; /**< MLU frequency lock status.*/
} cndevMLUFrequencyStatus_t;

/** MLU-Link port IP information.*/
typedef struct {
  int version;              /**< API version.*/
  __uint8_t ipVersion;      /**< IP version: IPV4 or IPV6.*/
  char ip[CNDEV_IP_ADDRESS_LEN]; /**< IP address of the MLU-Link port.*/
} cndevMLULinkPortIP_t;

/** CRC error information.*/
typedef struct {
  int version;                         /**< API version.*/
  __uint64_t die2dieCRCError;       	 /**< The count of D2D CRC errors.*/
  __uint64_t die2dieCRCErrorOverflow;  /**< The count of D2D CRC error overflows.*/
} cndevCRCInfo_t;

/** DDR information.*/
typedef struct {
  int version;                  /**< API version.*/
  __uint32_t dataWidth;         /**< DDR data width.*/
  __uint32_t bandWidth;         /**< DDR band width, unit: GB/s.*/
  __uint32_t bandWidthDecimal;  /**< The decimal of DDR band width, unit: GB/s.*/
} cndevDDRInfo_t;

/** Set frequency information.*/
typedef struct {
  __int32_t version;      /**< API version.*/
  __uint32_t minIpuFreq;  /**< Requested minimum MLU Core frequency in MHz.*/
  __uint32_t maxIpuFreq;  /**< Requested maximum MLU Core frequency in MHz.*/
} cndevSetIpuFrequency_t;

/** Process utilization.*/
typedef struct {
  __int32_t version;      /**< API version.*/
  __uint32_t pid;         /**< Process ID.*/
  __uint32_t ipuUtil;     /**< The MLU utilization of the process.*/
  __uint32_t jpuUtil;     /**< The JPU utilization of the process.*/
  __uint32_t vpuDecUtil;  /**< The VPU decoder utilization of the process.*/
  __uint32_t vpuEncUtil;  /**< The VPU encoder utilization of the process.*/
  __uint32_t memUtil;     /**< The memory utilization of the process.*/
} cndevProcessUtilization_t;

/** Supported IPU frequency.*/
typedef struct {
  __int32_t version;            /**< API version.*/
  __uint32_t supportedIpuFreq;  /**< Supported MLU Core frequency in MHz.*/
} cndevSupportedIpuFrequency_t;

/** Xid error types.*/
typedef enum {
  /**
   * No error returned.*/
  CNDEV_XID_NO_ERROR = 0,
  /**
   * General user application faults.*/
  CNDEV_XID_SW_NOTIFY_ERROR,
  /**
   * Internal micro-controller error.*/
  CNDEV_XID_MCU_ERROR,
  /**
   * DDR or HBM ECC error.*/
  CNDEV_XID_ECC_ERROR,
  /**
   * Commu or ipcm error.*/
  CNDEV_XID_RPC_ERROR,
  /**
   * Mem/Resource access error.*/
  CNDEV_XID_ILLEGAL_ACCESS_ERROR,
  /**
   * D2D CRC error.*/
  CNDEV_XID_D2D_CRC_ERROR,
  /**
   * D2D CRC error.*/
  CNDEV_XID_CRC_ERROR = CNDEV_XID_D2D_CRC_ERROR,
  /**
   * MLU-Link error.*/
  CNDEV_XID_MLULINK_ERROR,
  /**
   * MLU-Link error.*/
  CNDEV_XID_MLULINK_FATAL_ERROR = CNDEV_XID_MLULINK_ERROR,
  /**
   * HBM & DDR error.*/
  CNDEV_XID_HBM_ERROR,
  /**
   * Over-temperature.*/
  CNDEV_XID_OVER_TEMP_ERROR,
  /**
   * Previously halt.*/
  CNDEV_XID_PREV_HALT_ERROR,
  /**
   * PCIe replay error.*/
  CNDEV_XID_PCIE_REPLAY_ERROR,
  /**
   * Heartbeat error.*/
  CNDEV_XID_HEARTBEAT_ERROR,
  /**
   * ECC page retirement or row remapping.*/
  CNDEV_XID_PAGE_RETIREMENT_ERROR,
  /**
   * Fallen off the bus.*/
  CNDEV_XID_FALLEN_OFF_ERROR,
  /**
   * DBE error.*/
  CNDEV_XID_DBE_ECC_ERROR,
  /**
   * DMA fault.*/
  CNDEV_XID_PCIE_DMA_ERROR,
  /**
   * XPU core dump.*/
  CNDEV_XID_STOPPED_PROCESSING_ERROR,
  /**
   * Dev smmu error.*/
  CNDEV_XID_SMMU_ERROR,
  /**
   * MLU-Link replay error.*/
  CNDEV_XID_MLULINK_REPLAY_ERROR,
  /**
   * IPU reset error.*/
  CNDEV_XID_IPU_RESET_ERROR,
  /**
   * PCIe Link error.*/
  CNDEV_XID_PCIE_LINK_ERR,
  /**
   * PCIe bar addr error.*/
  CNDEV_XID_PCIE_CTX_ERR,
  /**
   * NOC bus error.*/
  CNDEV_XID_NOC_BUS_ERR,
  /**
   * Device system error.*/
  CNDEV_XID_DEV_SYS_0_ERR,
  /**
   * Firmware page address resource exceeded error.*/
  CNDEV_XID_PAGE_ADDRESS_RESOURCE_EXCEEDED_ERR,
  /**
   * Driver page retire resource exceeded error.*/
  CNDEV_XID_PAGE_RETIREMENT_RESOURCE_EXCEEDED_ERR,
  /**
   * SerDes CDR loss lock error.*/
  XID_MLULINK_CDR_LOSS,
  /**
   * MLU-Link overloaded with local faults in one second.*/
  XID_MLULINK_FATAL_ERR_HIGH_FREQ_LOCAL_FAULT,
  /**
   * MLU-Link overloaded with remote faults in one second.*/
  XID_MLULINK_FATAL_ERR_HIGH_FREQ_REMOTE_FAULT,
  /**
   * MLU-Link fatal error caused by local fault.*/
  XID_MLULINK_FATAL_ERR_LOCAL_FAULT,
  /**
   * MLU-Link fatal error caused by remote fault.*/
  XID_MLULINK_FATAL_ERR_REMOTE_FAULT,
  /**
   * The count of Xid errors.*/
  CNDEV_XID_MAX_COUNT,
} cndevXidEnum_t;

/** Xid error information.*/
typedef struct {
  __int32_t version;                          /**< API version.*/
  __uint32_t lastXid;                         /**< Last Xid error.*/
  __uint32_t totalXidCount;                   /**< Total Xid count.*/
  __uint32_t totalXid[CNDEV_XID_MAX_COUNT];   /**< Total Xids.*/
} cndevXidError_t;

/** Xid switch.*/
typedef struct {
  __int32_t version;                                        /**< API version.*/
  cndevEnableStatusEnum_t xidStatus[CNDEV_XID_MAX_COUNT];   /**< Xid switch status.*/
} cndevXidStatus_t;

/** For cleaning Xid status.*/
typedef struct {
  __int32_t version;                      /**< API version.*/
  __uint32_t selectAll;                   /**< Selects all Xid error switch status.*/
  __uint32_t selectedXid;                 /**< Selects one Xid error switch status.*/
  cndevEnableStatusEnum_t setXidStatus;   /**< Xid error switch status.*/
} cndevSetXid_t;

/** Power management limitations range information.*/
typedef struct {
  __int32_t version;                /**< API version.*/
  __uint16_t minPowerLimit;         /**< Minimum power setting, unit: W.*/
  __uint16_t minPowerLimitDecimal;  /**< Reserved. Decimal part is not supported.*/
  __uint16_t maxPowerLimit;         /**< Maximum power setting, unit: W.*/
  __uint16_t maxPowerLimitDecimal;  /**< Reserved. Decimal part is not supported.*/
} cndevPowerManagementLimit_t;

/** Power management limitations information.*/
typedef struct {
  __int32_t version;                /**< API version.*/
  __uint16_t powerLimit;            /**< Power management limitations, unit: W.*/
  __uint16_t powerLimitDecimal;     /**< Reserved. Decimal part is not supported.*/
} cndevPowerManagementLimitation_t;

/** Power management limitations range information.*/
typedef cndevPowerManagementLimit_t cndevPowerManagementLimitationRange_t;

/** Over temperature information.*/
typedef struct {
  __int32_t version;              /**< API version.*/
  __uint32_t powerOffCounter;     /**< The power-off times because of over-temperature, from loading driver to the current moment.*/
  __uint32_t underClockCounter;   /**< The underclocking times because of over-temperature, from loading driver to the current moment.*/
  /**< Over temperature causing poweroff will record the current temperature of IPU, the powerOffTemp
   * is the highest value in the records, unit: ℃.*/
  __int16_t powerOffTemp;
  /**< Over temperature causing underClock will record the current temperature of IPU, the underClockTemp
   * is the highest value in the records, unit: ℃.*/
  __int16_t underClockTemp;
} cndevOverTemperatureInfo_t;

/** Performance throttle reason information.*/
typedef struct {
  __int32_t version;                          /**< API version.*/
  cndevEnableStatusEnum_t tdp;                /**< Throttle reason by TDP.*/
  cndevEnableStatusEnum_t powerSetting;       /**< Throttle reason by setting power capping.*/
  cndevEnableStatusEnum_t clockSetting;       /**< Throttle reason by setting max frequency.*/
  cndevEnableStatusEnum_t frequencyLocked;    /**< Throttle reason by setting locked frequency.*/
  cndevEnableStatusEnum_t powerBrake;         /**< Throttle reason by power brake.*/
  cndevEnableStatusEnum_t thermalSlowdown;    /**< Throttle reason by thermal slowdown.*/
} cndevPerformanceThrottleReason_t;

/** Max performance information.*/
typedef struct {
  __uint32_t version;           /**< API version.*/
  __uint64_t int4Tensor;        /**< Peak INT4 Tensor MACs per cycle per core.*/
  __uint64_t int8Tensor;        /**< Peak INT8 Tensor MACs per cycle per core.*/
  __uint64_t int16Tensor;       /**< Peak INT16 Tensor MACs per cycle per core.*/
  __uint64_t fp16Tensor;        /**< Peak FP16 Tensor MACs per cycle per core.*/
  __uint64_t bf16Tensor;        /**< Peak BF16 Tensor MACs per cycle per core.*/
  __uint64_t fp32Tensor;        /**< Peak FP32 Tensor MACs per cycle per core.*/
  __uint64_t fp16Vector;        /**< Peak FP16 Vector OP per cycle per core.*/
  __uint64_t bf16Vector;        /**< Peak BF16 Vector OP per cycle per core.*/
  __uint64_t fp32Vector;        /**< Peak FP32 Vector OP per cycle per core.*/
  __uint64_t int8Vector;        /**< Peak INT8 Vector OP per cycle per core.*/
  __uint64_t int16Vector;       /**< Peak INT16 Vector OP per cycle per core.*/
} cndevDeviceMaxPerformance_t;

/** Compute mode types.*/
typedef enum {
  CNDEV_COMPUTEMODE_DEFAULT = 0,      /**< Default mode.*/
  CNDEV_COMPUTEMODE_EXCLUSIVE = 1,    /**< Exclusive mode.*/
  CNDEV_COMPUTEMODE_COUNT,            /**< The count of compute mode types.*/
} cndevComputeModeEnum_t;

/** Compute mode information.*/
typedef struct {
  __int32_t version;            /**< API version.*/
  cndevComputeModeEnum_t mode;  /**< The compute mode of device.*/
} cndevComputeMode_t;

/** PCIe replay information.*/
typedef struct {
  __int32_t version;        /**< API version.*/
  __uint32_t counter;       /**< The count of PCIe replays.*/
} cndevPcieReplayCounter_t;

/** SR-IOV mode information.*/
typedef struct {
  __int32_t version;                  /**< API version.*/
  cndevEnableStatusEnum_t sriovMode;  /**< The switch of SR-IOV mode.*/  
} cndevSRIOVMode_t;

/** MIM mode information.*/
typedef struct {
  __int32_t version;                  /**< API version.*/
  cndevEnableStatusEnum_t mimMode;    /**< The switch of MIM mode.*/ 
} cndevMimMode_t;

/** MIM instance placement information.*/
typedef struct {
  int version;                  /**< API version.*/
  int start;                    /**< The start placement of MIM instance.*/ 
  int size;                     /**< The size of MIM instance.*/ 
} cndevMluInstancePlacement_t;

/** MIM profile information.*/
typedef struct {
  int version;                                                /**< API version.*/
  int profileId;                                              /**< MIM instance profile ID.*/
  int mluCoreCount;                                           /**< The MIM core count of profile.*/
  int vpuCount;                                               /**< The VPU count of profile.*/
  int jpuCount;                                               /**< The JPU count of profile.*/
  int gdmaCount;                                              /**< The GDMA count of profile.*/
  int memorySize;                                             /**< The memory size of profile.*/
  char name[CNDEV_MLUINSTANCE_PROFILE_NAME_BUFFER_SIZE];
} cndevMluInstanceProfileInfo_t;

/** MIM instance information.*/
typedef struct {
  int version;                                                                /**< API version.*/
  cndevDevice_t parentDevice;                                                 /**< Parent device.*/
  int instanceId;                                                             /**< MIM instance Id.*/
  int profileId;                                                              /**< MIM instance created base on the profile.*/
  __uint8_t uuid[CNDEV_UUID_SIZE];                                            /**< The UUID of MIM instance.*/
  int domain;                                                                 /**< The domain ID of MIM instance.*/
  int bus;                                                                    /**< The bus ID of MIM instance.*/
  int device;                                                                 /**< The device ID of MIM instance.*/
  int function;                                                               /**< The function ID of MIM instance.*/
  cndevMluInstancePlacement_t placement;                                      /**< The placement information of MIM instance.*/
  int mluCoreCount;                                                           /**< The MLU core count of MIM instance.*/
  int vpuCount;                                                               /**< The VPU count of MIM instance.*/
  int jpuCount;                                                               /**< The JPU count of MIM instance.*/
  int gdmaCount;                                                              /**< The GDMA count of MIM instance.*/
  int memorySize;                                                             /**< The memory size of MIM instance.*/
  char profileName[CNDEV_MLUINSTANCE_PROFILE_NAME_BUFFER_SIZE];               /**< The name of MIM profile.*/
  char devNodeName[CNDEV_MLUINSTANCE_DEVICE_NODE_NAME_BUFFER_SIZE];           /**< The device node of MIM instance.*/
  char ipcmDevNodeName[CNDEV_MLUINSTANCE_DEVICE_NODE_NAME_BUFFER_SIZE];       /**< The ipcm node of MIM instance.*/
  char instanceName[CNDEV_MLUINSTANCE_DEVICE_NODE_NAME_BUFFER_SIZE];          /**< The name of MIM instance.*/
  unsigned int reserved[128];                                                 /**< Reserved.*/
} cndevMluInstanceInfo_t;

/** Parity error information.*/
typedef struct {
  int version;  /**< API version.*/
  int counter;  /**< The count of parity errors.*/
} cndevParityError_t;

/** Docker parameter information.*/
typedef struct {
  int version;                                /**< API version.*/
  char dockerParam[CNDEV_NAME_BUFFER_SIZE];   /**< The parameter of run docker.*/
} cndevDockerParam_t;

/** sMLU mode information.*/
typedef struct {
  int version;                      /**< API version.*/
  cndevEnableStatusEnum_t smluMode; /**< The switch of sMLU mode.*/ 
} cndevSMLUMode_t;

/** sMLU profile mluQuota and memorySize information for creating new profile.*/
typedef struct {
  int version;            /**< API version.*/
  __uint32_t mluQuota;    /**< The utilization of sMLU profile.*/
  __uint64_t memorySize;  /**< The memory size of sMLU profile, unit: B.*/
} cndevSMluSet_t;

/** sMLU array indices for mluQuota and memorySize.*/
typedef enum {
  CNDEV_SMLU_MAX = 0,     /**< sMLU maximum information.*/
  CNDEV_SMLU_USAGE = 1,   /**< sMLU usage information.*/
  CNDEV_SMLU_ITEM_COUNT,  /**< The count of sMLU information.*/
} cndevSMluItemEnum_t;

/** sMLU instance information.*/
typedef struct {
  int version;                                                          /**< API version.*/
  int profileId;                                                        /**< sMLU instance created base on the profile.*/
  int instanceId;                                                       /**< sMLU instance ID.*/
  __uint8_t uuid[CNDEV_UUID_SIZE];                                      /**< The UUID of sMLU instance.*/
  int domain;                                                           /**< The domain of sMLU instance.*/
  int bus;                                                              /**< The bus of sMLU instance.*/
  int device;                                                           /**< The device of sMLU instance.*/
  int function;                                                         /**< The function of sMLU instance.*/
  char profileName[CNDEV_MLUINSTANCE_PROFILE_NAME_BUFFER_SIZE];         /**< The name of sMLU profile.*/
  char instanceName[CNDEV_MLUINSTANCE_PROFILE_NAME_BUFFER_SIZE];        /**< The name of sMLU instance.*/
  char devNodeName[CNDEV_MLUINSTANCE_DEVICE_NODE_NAME_BUFFER_SIZE];     /**< The device node of sMLU instance.*/
  __uint32_t mluQuota[CNDEV_SMLU_ITEM_COUNT];                           /**< The utilization of sMLU instance.*/
  __uint64_t memorySize[CNDEV_SMLU_ITEM_COUNT];                         /**< The memory size of sMLU instance, unit: B.*/
  unsigned int reserved[128];                                           /**< Reserved.*/
} cndevSMluInfo_t;

/** sMLU instance ID information.*/
typedef struct {
  int version;          /**< API version.*/
  int count;            /**< The profile count of sMLU.*/
  int profileId[128];   /**< The profile ID information of sMLU.*/
} cndevSMluProfileIdInfo_t;

/** sMLU profile information.*/
typedef struct {
  int version;                                                  /**< API version.*/
  int profileId;                                                /**< Profile ID.*/
  char name[CNDEV_MLUINSTANCE_PROFILE_NAME_BUFFER_SIZE];        /**< Profile name.*/
  __uint32_t totalCapacity;                                     /**< The total count of sMLU instances that were created base on the profile.*/
  __uint32_t remainCapacity;                                    /**< The remain count of sMLU instances that were created base on the profile.*/
  __uint32_t mluQuota[CNDEV_SMLU_ITEM_COUNT];                   /**< MluQuota[0] is the utilization of sMLU profile, mluQuota[1] is reserved.*/
  __uint64_t memorySize[CNDEV_SMLU_ITEM_COUNT];                 /**< MemorySize[0] is the memory size of sMLU profile, unit: B, memorySize[1] is reserved.*/
} cndevSMluProfileInfo_t;

/** Xid error V2.*/
typedef struct {
  __uint32_t version;                           /**< API version.*/
  __uint64_t xidSupported[CNDEV_XID_MAX_COUNT]; /**< indicates whether the MLU Driver supports.*/
  __uint64_t xidCount[CNDEV_XID_MAX_COUNT];     /**< Error count.*/
} cndevXidErrorV2_t;

/** Memory ECC information.*/
typedef struct {
  int version;                                  /**< API version.*/
  __uint32_t volatileSramEccSbeCounter;         /**< The count of volatile SRAM ECC SBE. This number is accumulated count from loading MLU Driver to the current time.*/
  __uint32_t volatileSramEccDbeCounter;         /**< The count of volatile SRAM ECC DBE. This number is accumulated count from loading MLU Driver to the current time.*/
  __uint32_t volatileSramEccParityCounter;      /**< The count of volatile SRAM ECC PARITY. This number is accumulated count from loading MLU Driver to the current time.*/
  __uint32_t volatileDramEccSbeCounter;         /**< The count of volatile DRAM ECC SBE. This number is accumulated count from loading MLU Driver to the current time.*/
  __uint32_t volatileDramEccDbeCounter;         /**< The count of volatile DRAM ECC DBE. This number is accumulated count from loading MLU Driver to the current time.*/
  __uint64_t aggregateDramEccSbeCounter;        /**< The count of aggregate DRAM ECC SBE. This number is accumulated count from the manufacturing date to the current time.*/
  __uint64_t aggregateDramEccDbeCounter;        /**< The count of aggregate DRAM ECC DBE. This number is accumulated count from the manufacturing date to the current time.*/
  __uint64_t aggregateSramEccSbeCounter;        /**< The count of aggregate SRAM ECC SBE. This number is accumulated count from the manufacturing date to the current time.*/
  __uint64_t aggregateSramEccDbeCounter;        /**< The count of aggregate SRAM ECC DBE. This number is accumulated count from the manufacturing date to the current time.*/
  __uint64_t aggregateSramEccParityCounter;     /**< The count of aggregate SRAM ECC PARITY. This number is accumulated count from the manufacturing date to the current time.*/
  bool aggregateSramEccThresholdExceeded;       /**< The identifier that indicates whether the aggregate SRAM ECCs exceeded the threshold.*/
  __uint64_t reserved[11];                      /**< Reserved.*/
} cndevMemEccCounter_t;

typedef struct {
  __uint64_t total_size;            /**< Total BAR4 Memory in bytes.*/
  __uint64_t used_size;             /**< Used BAR4 Memory in bytes.*/
  __uint64_t free_size;             /**< Free BAR4 Memory in bytes.*/
} cndevBAR4Memory_t;

/** Clock types, all speeds are in MHz.*/
typedef enum {
  CNDEV_CLOCK_IPU = 0,              /**< IPU clock domain.*/
} cndevClockType_t;

/** Event information.*/
typedef struct {
  __uint32_t version;                           /**< API version.*/
  cndevDevice_t device;                         /**< Specific device where the event occurred.*/
  __uint64_t eventData;                         /**< Stores Xid error data for the device.*/
  __uint64_t eventType;                         /**< Stores Xid type for the device.*/
  __uint64_t timestamp;                         /**< Stores Xid timestamp for the device.*/
  __uint32_t computeInstanceId;                 /**< When MIM is enabled, the event is attributable to a CPU.*/
  __uint32_t mluInstanceId;                     /**< When MIM is enabled, the event is attributable to an MLU.*/
  __uint64_t reserved[8];                       /**< Reserved.*/
} cndevEventData_t;

/** Device voltage information.*/
typedef struct {
  short ipuCoreVoltage;   /**< The voltage of IPU core, unit mV.*/
  short socVoltage;       /**< The voltage of SoC, unit mV.*/
  short hbmVccVoltage;    /**< Reserved.*/
  short hbmVddVoltage;    /**< The voltage of HBM, unit mV.*/
  short reserved[16];     /**< Reserved.*/
} cndevVoltageInfo_t;

/** Device current information.*/
typedef struct {
  int ipuCoreCurrent;   /**< The current of IPU core, unit mA.*/
  int socCurrent;       /**< The current of SoC, unit mA.*/
  int hbmCurrent;       /**< The current of HBM, unit mA.*/
  int reserved[16];     /**< Reserved.*/
} cndevCurrentInfo_t;

/** The event handle data type.*/
typedef void * cndevEventHandle;

#define cndevEventTypeAll      0xffffffffffffffffULL
#define cndevEventTypeNone     0x0000000000000000ULL
#define cndevEventTypeXidError 0x0000000000000001ULL

/** The callback function for traverse topology.*/
typedef int (*CNDEV_TRAVERSE_CALLBACK)(cndevTopologyNode_t *current, void *userdata);
/**
 * @brief Completes initialization work, and check whether the API version and the MLU Driver version could be supported.
 * 
 * @param[in] reserved Initialization flag for CNDev.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_NO_DRIVER ,
 * - ::CNDEV_ERROR_LOW_DRIVER_VERSION,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_NO_DEVICES
 */
CNDEV_EXPORT
cndevRet_t cndevInit(int reserved);
/**
 * @brief Completes exit work.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_UNKNOWN
 */
CNDEV_EXPORT
cndevRet_t cndevRelease();

/**
 * @brief Gets the amount of devices.
 *
 * @param[out] cardNum The pointer which stores the amount of devices after the function ends.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION
 */
CNDEV_EXPORT
cndevRet_t cndevGetDeviceCount(cndevCardInfo_t *cardNum);

/**
 * @brief Gets the device ID information of PCIe.
 *
 * @param[in,out] deviceInfo The pointer which stores the information of the device ID after the function ends.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION
 */
CNDEV_EXPORT
cndevRet_t cndevGetPCIeInfo(cndevPCIeInfo_t *deviceInfo, cndevDevice_t device);

/**
 * @brief Gets the memory information of the device.
 *
 * @param[in,out] memInfo The pointer which stores the information of the devices' memory after the function ends.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetMemoryUsage(cndevMemoryInfo_t *memInfo, cndevDevice_t device);

/**
 * @brief Gets the memory information of the device.
 *
 * @param[in,out] memInfo The pointer which stores the information of the devices' memory after the function ends.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetMemoryUsageV2(cndevMemoryInfoV2_t *memInfo, cndevDevice_t device);

/**
 * @brief Gets the information of device's MCU version and MLU Driver version.
 *
 * @param[in,out] versInfo The pointer which stores the information of the devices' MCU version and MLU Driver.
 * version after the function ends.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION
 */
CNDEV_EXPORT
cndevRet_t cndevGetVersionInfo(cndevVersionInfo_t *versInfo, cndevDevice_t device);

/**
 * @brief Gets the information of the device's ECC.
 *
 * @param[in,out] eccInfo The pointer which stores the information of the device's ECC.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetECCInfo(cndevECCInfo_t *eccInfo, cndevDevice_t device);

/**
 * @brief Sets the mode of the device's ECC.
 *
 * @param[in,out] eccMode The pointer which stores the mode of the device's ECC.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_NO_PERMISSION
 */
CNDEV_EXPORT
cndevRet_t cndevSetEccMode(cndevEccMode_t *eccMode, cndevDevice_t device);

/**
 * @brief Gets the ECC mode of the device.
 *
 * @param[in,out] currentMode The pointer which stores the current ECC mode of the device.
 * @param[in,out] currentMode The pointer which stores the pending ECC mode of the device.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetEccMode(cndevEccMode_t *currentMode, cndevEccMode_t *pendingMode, cndevDevice_t device);

/**
 * @brief Gets the ECC count of the device.
 *
 * @param[in,out] eccCounter The pointer which stores the ECC count of the device.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT 
cndevRet_t cndevGetMemEccCounter(cndevMemEccCounter_t *eccCounter, cndevDevice_t device);

/**
 * @brief Gets the information of device's power consumption.
 *
 * @param[in,out] powerInfo The pointer which stores the information of the device's power consumption after the
 * function ends.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetPowerInfo(cndevPowerInfo_t *powerInfo, cndevDevice_t device);


/**
 * @brief Gets the information of device's power consumption. @p maxPower in @p powerInfo greater than 0 is valid.
 *
 * @param[in,out] powerInfo The pointer which stores the information of the device's power consumption after the
 * function ends.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetPowerInfoV2(cndevPowerInfoV2_t *powerInfo, cndevDevice_t device);

/**
 * @brief Gets the information of the device's frequency.
 *
 * @param[in,out] freqInfo The pointer which stores the information of the device's frequency after the function ends.
 * @param[in] device The identifier of the target device.
 * 
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetFrequencyInfo(cndevFrequencyInfo_t *freqInfo, cndevDevice_t device);

/**
 * @brief Gets the information of the device's temperature.
 *
 * @param[in,out] tempInfo The pointer which stores the information of the device's temperature after the function ends.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetTemperatureInfo(cndevTemperatureInfo_t *tempInfo, cndevDevice_t device);

/**
 * @brief Gets the information of the device's utilization.
 *
 * @param[in,out] utilInfo The pointer which stores the information of the  devices' utilization after the function ends.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetDeviceUtilizationInfo(cndevUtilizationInfo_t *utilInfo, cndevDevice_t device);

/**
 * @brief Gets the information of the device's fan speed.
 *
 * @param[in,out] fanInfo The pointer which stores the information of the devices' fan speed after the function ends.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetFanSpeedInfo(cndevFanSpeedInfo_t *fanInfo, cndevDevice_t device);

/**
 * @brief Gets the information of the device's processes.
 *
 * @param[in,out] infoCount The size of the space which the user allocates for storing the information of processes. At the same time, the
 * parameter The pointer which stores a pointer to the size of a space which is suit to store all information after the function ends.
 * @param[in,out] procInfo The pointer of the space which the user allocates for saving the information of processes.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_INSUFFICIENT_SPACE,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION
 */
CNDEV_EXPORT
cndevRet_t cndevGetProcessInfo(unsigned *infoCount, cndevProcessInfo_t *procInfo, cndevDevice_t device);

/**
 * @brief Gets the information of the CNDev library version.
 *
 * @param[in,out] libVerInfo The pointer which stores the information of the CNDev library version after the
 *function ends.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION
 */
CNDEV_EXPORT
cndevRet_t cndevGetLibVersion(cndevLibVersionInfo_t *libVerInfo);

/**
 * @brief Gets the count of the device's cores which the user selects.
 *
 * @param[in,out] cardCoreCount The pointer which stores the count of the devices' cores after the function ends.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION
 */
CNDEV_EXPORT
cndevRet_t cndevGetCoreCount(cndevCardCoreCount_t *cardCoreCount, cndevDevice_t device);

/**
 * @brief Gets the count of the device's clusters which the user selects.
 *
 * @param[in,out] clusCount The pointer which stores the count of the device's clusters after the function ends.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION
 */
CNDEV_EXPORT
cndevRet_t cndevGetClusterCount(cndevCardClusterCount_t *clusCount, cndevDevice_t device);

/**
 * @brief Gets the lowest MLU Driver version which the CNDev library supports.
 * 
 * @param[in,out] versInfo The pointer which stores the lowest MLU Driver version after the function ends.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION
 */
CNDEV_EXPORT
cndevRet_t cndevGetLowestSupportDriverVersion(cndevVersionInfo_t *versInfo);

/**
 * @brief Gets the index of device's name.
 *
 * @param[in,out] cardName The pointer which stores the index of a device's name after the function ends.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION
 */
CNDEV_EXPORT
cndevRet_t cndevGetCardName(cndevCardName_t *cardName, cndevDevice_t device);

/**
 * @brief Translates the index of a device's name to the string of the device's name.
 *
 * @param cardName The index of a device's name.
 *
 * @return
 * - "Unknown" if the string of the device's name can't be found, otherwise the string of the device's name will be returned.
 */
CNDEV_EXPORT
const char *cndevGetCardNameString(cndevNameEnum_t cardName);

/**
 * @brief Translates the index of a device's name to the string of the device's name.
 *
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - The string of the device's name by device ID.
 */
CNDEV_EXPORT
const char *cndevGetCardNameStringByDevId(cndevDevice_t device);

/**
 * @brief Gets the SN (serial number) of the device.
 *
 * @param[in,out] cardSN The pointer which stores the SN of the device after the function ends.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION
 */
CNDEV_EXPORT
cndevRet_t cndevGetCardSN(cndevCardSN_t *cardSN, cndevDevice_t device);

/**
 * @brief Gets the PN (part number) of the device.
 *
 * @param[in,out] cardSN The pointer which stores the PN of the device after the function ends.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION
 */
CNDEV_EXPORT
cndevRet_t cndevGetCardPartNumber(cndevCardPartNumber_t *cardPN, cndevDevice_t device);

/**
 * @brief Gets the PCIe throughput information.
 * Gets the throughput in the past 20ms.
 *
 * @param[in,out] pciethroughput The pointer which stores PCIe read and write throughput.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetPCIethroughput(cndevPCIethroughput_t *pciethroughput, cndevDevice_t device);

/**
 * @brief Gets the PCIe throughput information.
 * Gets the total throughput and throughput in the past 20ms.
 *
 * @param[in,out] pciethroughput The pointer which stores PCIe throughput information.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetPCIethroughputV2(cndevPCIethroughputV2_t *pciethroughput, cndevDevice_t device);

/**
 * @brief Gets device related CPU affinity.
 *
 * @param[in,out] affinity The pointer which stores CPU affinity info.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetDeviceAffinity(cndevAffinity_t *affinity, cndevDevice_t device);

/**
 * @brief Clears current thread's CPU affinity, set to default.
 *
 * @param[in] version The CNDev version.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevClearCurrentThreadAffinity(int version, cndevDevice_t device);

/**
 * @brief Sets current thread's CPU affinity to device related CPUs.
 *
 * @param[in] version The CNDev version.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevSetCurrentThreadAffinity(int version, cndevDevice_t device);

/**
 * @brief Gets two devices' relationship.
 *
 * @param[in,out] relationship The pointer which stores two devices' relationship.
 * @param[in] device1 The identifier of the target device1.
 * @param[in] device2 The identifier of the target device2.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevTopologyGetRelationship(cndevTopologyRelationship_t *relationship, cndevDevice_t device1, cndevDevice_t device2);

/**
 * @brief Gets devid nearest devices by relationship.
 *
 * @param[in] version The CNDev version.
 * @param[in] rel Specified relationship.
 * @param[in,out] count DeviceArray's size.
 * @param[out] deviceArray The pointer which stores related devices handle.
 * @param[in] device The count of devices, starting from 0.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INSUFFICIENT_SPACE,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevTopologyGetNearestDevices(int version, cndevTopologyRelationshipEnum_t rel, __uint64_t *count,
                                          __uint64_t *deviceArray, cndevDevice_t device);

/**
 * @brief Gets two devices' relationship.
 *
 * @param version The CNDev version.
 * @param cpuId Specified CPU ID.
 * @param[in,out] count DeviceArray's size.
 * @param[out] deviceArray The pointer which stores related devices handle.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INSUFFICIENT_SPACE,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevTopologyGetCpuRelatedDevices(int version, int cpuId, __uint64_t *count, __uint64_t *deviceArray);

/**
 * @brief Gets the current PCI information.
 *
 * @param[in,out] currentPCIInfo The pointer to the structure storing the current PCI information.
     after the function ends.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
*/
CNDEV_EXPORT
cndevRet_t cndevGetCurrentPCIInfo(cndevCurrentPCIInfo_t *currentPCIInfo, cndevDevice_t device);

/**
 * @brief Gets the max PCI information.
 *
 * @param[in,out] maxPCIInfo The pointer to the structure storing the max PCI information.
     after the function ends.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
*/
CNDEV_EXPORT
cndevRet_t cndevGetMaxPCIInfo(cndevPCIInfo_t *maxPCIInfo, cndevDevice_t device);

/**
 * @brief Gets two nodes' relationship.
 *
 * @param[out] relationship The pointer which stores two devices' relationship.
 * @param[in] node1 The topology node.
 * @param[in] node2 The topology node.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevTopologyGetRelationshipByNode(cndevTopologyRelationship_t *relationship, cndevTopologyNode_t *node1,
                                              cndevTopologyNode_t *node2);

/**
 * @brief Gets a topology tree node by BDF.
 *
 * @param[in] version The CNDev version.
 * @param[out] treeNode The target topology tree node.
 * @param[in] domain  The domain number of PCI device.
 * @param[in] bus The bus number of PCI device.
 * @param[in] device The slot number of PCI device.
 * @param[in] function The function number of PCI device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetNodeByBDF(int version, cndevTopologyNode_t **treeNode, unsigned int domain, unsigned int bus,
                             unsigned int device, unsigned int function);

/**
 * @brief Gets the device ID by BDF.
 *
 * @param[in] version The CNDev version.
 * @param[out] dev The identifier of the target device.
 * @param[in] domain  The domain number of PCI device.
 * @param[in] bus The bus number of PCI device.
 * @param[in] device The slot number of PCI device.
 * @param[in] function The function number of PCI device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetDevIdByBDF(int version, cndevDevice_t *dev, unsigned int domain, unsigned int bus, unsigned int device,
                              unsigned int function);

/**
 * @brief Gets a topology tree node by device ID.
 *
 * @param[in] version The CNDev version.
 * @param[out] treeNode The target topology tree node.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetNodeByDevId(int version, cndevTopologyNode_t **treeNode, cndevDevice_t device);

/**
 * @brief Gets the virtual root node of topology tree.
 *
 * @param[in] version The CNDev version.
 * @param[out] root The virtual root node of topology tree.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevTopologyGetVirtualRootNode(int version, cndevTopologyNode_t **root);

/**
 * @brief Traverses the topology tree with a callback function.
 *
 * @param[in] version The CNDev version.
 * @param[in] callback The name of callback function, traverse the topology tree while the return value of callback function is 1
 *   while the return value of callback function is zero, the traverse tree function break.
 * @param[in] userdata The parameter of callback function.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevTopologyTraverseTree(int version, CNDEV_TRAVERSE_CALLBACK callback, void *userdata);

/**
 * @brief Gets the capability info of tree node.
 *
 * @param[in,out] capability The capability info of tree node.
 * @param[in] treeNode The target tree node.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetNodeCapabilityInfo(cndevCapabilityInfo_t *capability, cndevTopologyNode_t *treeNode);

/**
 * @brief Gets tree nodes by device name.
 *
 * @param[in] version The CNDev version.
 * @param[in,out] count The nodeArray's size.
 * @param[in,out] nodeArray The pointer which stores related devices' node.
 * @param[in] deviceName The name of PCI device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INSUFFICIENT_SPACE,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetNodeByDeviceName(int version, int *count, cndevTopologyNode_t *nodeArray, const char *deviceName);

/**
 * @brief Gets the healthy state of the device.
 *
 * @param[in,out] cardHealthState The pointer to the structure storing the HP of the device after the function ends, 1 means health, 0 means sick.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION
 */
CNDEV_EXPORT
cndevRet_t cndevGetCardHealthState(cndevCardHealthState_t* cardHealthState, cndevDevice_t device);

/**
 * @brief Gets the healthy state of the device with list of incidents.
 *
 * @param[in,out] cardHealthState The pointer to the structure storing the health result.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID
 */
CNDEV_EXPORT
cndevRet_t cndevGetCardHealthStateV2(cndevCardHealthStateV2_t* cardHealthState, cndevDevice_t device);

/**
 * @brief Gets the heartbeat count of the device.
 *
 * @param[in,out] cardHeartbeatCount The pointer to the structure storing the heartbeat count.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION
 */
CNDEV_EXPORT
cndevRet_t cndevGetCardHeartbeatCount(cndevCardHeartbeatCount_t* cardHeartbeatCount, cndevDevice_t device);

/**
 * @brief Gets the PCIe switch link speed.
 *
 * @param[in,out] linkspeed The pointer to the structure storing the PCIe switch link speed.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION
 */
CNDEV_EXPORT
cndevRet_t cndevGetLowestLinkSpeed(cndevLinkSpeed_t* linkspeed, cndevDevice_t device);

/**
 * @brief Gets the JPU codec utilization.
 *
 * @param jpu_util The pointer to the structure storing the JPU codec utilization.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetImageCodecUtilization(cndevImageCodecUtilization_t* jpuutil, cndevDevice_t device);

/**
 * @brief Gets the VPU codec utilization.
 *
 * @param[in,out] vpu_util The pointer to the structure storing the VPU codec utilization.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetVideoCodecUtilization(cndevVideoCodecUtilization_t* vpuutil, cndevDevice_t device);

/**
 * @brief Gets the fast alloc information.
 *
 * @param[in,out] fastalloc The pointer to the structure storing the fast alloc total memory and free memory.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetFastAlloc(cndevFastAlloc_t* fastalloc, cndevDevice_t device);

/**
 * @brief Gets the NUMA node ID of tree node.
 *
 * @param[in,out] numaNodeId The NUMA node ID of tree node.
 * @param[in] treeNode The target tree node.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetNUMANodeIdByTopologyNode(cndevNUMANodeId_t* numaNodeId, cndevTopologyNode_t* treeNode);

/**
 * @brief Gets the scaler utilization.
 *
 * @param[in,out] scalerutil The pointer to the structure storing the scaler utilization.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetScalerUtilization(cndevScalerUtilization_t* scalerutil, cndevDevice_t device);

/**
 * @brief Gets the codec turbo mode.
 *
 * @param[in,out] codecTurbo The pointer to the structure storing the codec turbo information.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetCodecTurbo(cndevCodecTurbo_t* codecTurbo, cndevDevice_t device);

/**
 * @brief Gets the memorydie count.
 *
 * @param[in,out] memorydiecount The pointer to the structure storing the memorydie count.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetMemoryDieCount(cndevCardMemoryDieCount_t* memorydiecount, cndevDevice_t device);

/**
 * @brief Gets the QSFP-DD status, only valid on MLU290.
 *
 * @param[in,out] qddstatus The pointer to the structure storing the QSFP-DD status.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetQsfpddStatus(cndevQsfpddStatus_t* qddstatus, cndevDevice_t device);

/**
 * @brief Gets the MLU-Link version.
 *
 * @param[in,out] version The pointer to the structure storing the MLU-Link version.
 * @param[in] device The identifier of the target device.
 * @param[in] link The count of the ports which the user selects, starting from 0.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_INVALID_LINK
 */
CNDEV_EXPORT
cndevRet_t cndevGetMLULinkVersion(cndevMLULinkVersion_t *version, cndevDevice_t device, int link);
/**
 * @brief Gets the MLU-Link status
 *
 * @param[in,out] status The pointer to the structure storing the MLU-Link status.
 * @param[in] device The identifier of the target device.
 * @param[in] link The count of the ports which the user selects, starting from 0.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_INVALID_LINK
 */
CNDEV_EXPORT
cndevRet_t cndevGetMLULinkStatus(cndevMLULinkStatus_t *status, cndevDevice_t device, int link);
/**
 * @brief Gets the Presence, SerDes and MAC status of MLU-Link.
 *
 * @param[in,out] status The pointer to the structure storing the MLU-Link status.
 * @param[in] device The identifier of the target device.
 * @param[in] link The index of the selected port, starting from 0.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_INVALID_LINK
 */
CNDEV_EXPORT
cndevRet_t cndevGetMLULinkStatusV2(cndevMLULinkStatusV2_t *status, cndevDevice_t device, int link);
/**
 * @brief Gets the optical information of MLU-Link.
 *
 * @param[in,out] optical The pointer to the structure storing the optical information of MLU-Link.
 * @param[in] device The identifier of the target device.
 * @param[in] link The index of the selected port, starting from 0.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_NOT_FOUND,
 * - ::CNDEV_ERROR_INVALID_LINK
 */
CNDEV_EXPORT
cndevRet_t cndevGetOpticalInfo(cndevOpticalInfo_t *optical, cndevDevice_t device, int link);
/**
 * @brief Gets the MLU-Link speed.
 *
 * @param[in,out] speed The pointer to the structure storing the MLU-Link speed.
 * @param[in] device The identifier of the target device.
 * @param[in] link The count of the ports which the user selects, starting from 0.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_INVALID_LINK
 */
CNDEV_EXPORT
cndevRet_t cndevGetMLULinkSpeedInfo(cndevMLULinkSpeed_t *speed, cndevDevice_t device, int link);
/**
 * @brief Gets the MLU-Link capability, only valid on MLU370.
 *
 * @param[in,out] capability The pointer to the structure storing the MLU-Link capability.
 * @param[in] device The identifier of the target device.
 * @param[in] link The count of the ports which the user selects, starting from 0.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_INVALID_LINK
 */
CNDEV_EXPORT
cndevRet_t cndevGetMLULinkCapability(cndevMLULinkCapability_t *capability, cndevDevice_t device, int link);
/**
 * @brief Gets the MLU-Link count information.
 *
 * @param[in,out] count The pointer to the structure storing the MLU-Link count information.
 * @param[in] device The identifier of the target device.
 * @param[in] link The count of the ports which the user selects, starting from 0.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_INVALID_LINK
 *
 * @note
 * - This function will be deprecated in a future release. Use ::cndevGetMLULinkBasicCounter(),
 *   ::cndevGetMLULinkCongestionCtrlCounter(), ::cndevGetMLULinkErrorCounter(), ::cndevGetMLULinkFlowCtrlCounter(),
 *   ::cndevGetMLULinkEventCounter(), and ::cndevGetMLULinkTaskStatsCounter() instead.
 */
CNDEV_EXPORT
cndevRet_t cndevGetMLULinkCounter(cndevMLULinkCounter_t *count, cndevDevice_t device, int link);
/**
 * @brief Gets the MLU-Link statistics information.
 *
 * @param[out] count The pointer to the structure storing the MLU-Link statistics information.
 * @param[in] device The identifier of the target device.
 * @param[in] link The count of the ports which the user selects, starting from 0.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_INVALID_LINK
 */
CNDEV_EXPORT
cndevRet_t cndevGetMLULinkCounterExt(cndevMLULinkCounterExt_t *count, cndevDevice_t device, int link);
/**
 * @brief Resets the MLU-Link count.
 *
 * @param[in,out] setcount The pointer to the structure storing the MLU-Link count.
 * @param[in] device The identifier of the target device.
 * @param[in] link The count of the ports which the user selects, starting from 0.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_INVALID_LINK,
 * - ::CNDEV_ERROR_NO_PERMISSION
 */
CNDEV_EXPORT
cndevRet_t cndevResetMLULinkCounter(cndevMLULinkSetCounter_t *setcount, cndevDevice_t device, int link);
/**
 * @brief Gets the basic counter of an MLU-Link.
 *
 * @param[out] count The pointer to the structure storing the MLU-Link statistics information.
 * @param[in] device The identifier of the target device.
 * @param[in] link The count of the ports which the user selects, starting from 0.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_INVALID_LINK
 */
CNDEV_EXPORT
cndevRet_t cndevGetMLULinkBasicCounter(cndevMLULinkBasicCounter_t *counter, cndevDevice_t device, int link);
/**
 * @brief Gets the congestion control counter of an MLU-Link.
 *
 * @param[out] count The pointer to the structure storing the MLU-Link statistics information.
 * @param[in] device The identifier of the target device.
 * @param[in] link The count of the ports which the user selects, starting from 0.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_INVALID_LINK
 */
CNDEV_EXPORT
cndevRet_t cndevGetMLULinkCongestionCtrlCounter(cndevMLULinkCongestionCtrlCounter_t *counter, cndevDevice_t device, int link);
/**
 * @brief Gets the error counter of an MLU-Link.
 *
 * @param[out] count The pointer to the structure storing the MLU-Link statistics information.
 * @param[in] device The identifier of the target device.
 * @param[in] link The count of the ports which the user selects, starting from 0.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_INVALID_LINK
 */
CNDEV_EXPORT
cndevRet_t cndevGetMLULinkErrorCounter(cndevMLULinkErrorCounter_t *counter, cndevDevice_t device, int link);
/**
 * @brief Gets the flow control counter of an MLU-Link.
 *
 * @param[out] count The pointer to the structure storing the MLU-Link statistics information.
 * @param[in] device The identifier of the target device.
 * @param[in] link The count of the ports which the user selects, starting from 0.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_INVALID_LINK
 */
CNDEV_EXPORT
cndevRet_t cndevGetMLULinkFlowCtrlCounter(cndevMLULinkFlowCtrlCounter_t *counter, cndevDevice_t device, int link);
/**
 * @brief Gets the event counter of an MLU-Link.
 *
 * @param[out] count The pointer to the structure storing the MLU-Link statistics information.
 * @param[in] device The identifier of the target device.
 * @param[in] link The count of the ports which the user selects, starting from 0.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_INVALID_LINK
 */
CNDEV_EXPORT
cndevRet_t cndevGetMLULinkEventCounter(cndevMLULinkEventCounter_t *counter, cndevDevice_t device, int link);
/**
 * @brief Gets the task statistics counter of an MLU-Link.
 *
 * @param[out] count The pointer to the structure storing the MLU-Link statistics information.
 * @param[in] device The identifier of the target device.
 * @param[in] link The count of the ports which the user selects, starting from 0.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_INVALID_LINK
 */
CNDEV_EXPORT
cndevRet_t cndevGetMLULinkTaskStatsCounter(cndevMLULinkTaskStatsCounter_t *counter, cndevDevice_t device, int link);
/**
 * @brief Disables an MLU-Link. This will take effect after resetting the device.
 *
 * @param[in] device The identifier of the target device.
 * @param[in] link The count of the ports which the user selects, starting from 0.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_INVALID_LINK
 */
CNDEV_EXPORT
cndevRet_t cndevDisableMLULink(cndevDevice_t device, int link);
/**
 * @brief Enables an MLU-Link. This will take effect after resetting the device.
 *
 * @param[in] device The identifier of the target device.
 * @param[in] link The count of the ports which the user selects, starting from 0.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_INVALID_LINK
 */
CNDEV_EXPORT
cndevRet_t cndevEnableMLULink(cndevDevice_t device, int link);
/**
 * @brief Gets the MLU-Link remote information.
 *
 * @param[in,out] remoteinfo The pointer to the structure storing the MLU-Link remote information.
 * @param[in] device The identifier of the target device.
 * @param[in] link The count of the ports which the user selects, starting from 0.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_INVALID_LINK
 */
CNDEV_EXPORT
cndevRet_t cndevGetMLULinkRemoteInfo(cndevMLULinkRemoteInfo_t *remoteinfo, cndevDevice_t device, int link);
/**
 * @brief Gets the MLU-Link devices' sn.
 *
 * @param[in,out] devinfo The pointer to the structure storing the MLU-Link devices sn.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetMLULinkDevSN(cndevMLULinkDevSN_t *devinfo, cndevDevice_t device);
/**
 * @brief Gets the NUMA node ID of tree node by device ID.
 *
 * @param[in,out] numaNodeId The NUMA node ID of tree node.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetNUMANodeIdByDevId(cndevNUMANodeId_t* numaNodeId, cndevDevice_t device);
/**
 * @brief Gets the chassis information.
 *
 * @param[in,out] chassisinfo The pointer to the structure storing the chassis information.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetChassisInfo(cndevChassisInfo_t* chassisinfo, cndevDevice_t device);
/**
 * @brief Gets the chassis information V2.
 *
 * @param[in,out] chassisinfo The pointer to the structure storing the chassis information.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetChassisInfoV2(cndevChassisInfoV2_t* chassisinfo, cndevDevice_t device);
/**
 * @brief Gets the PCIe firmware version information.
 *
 * @param[in,out] version The pointer to the structure storing the PCIe firmware version information.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetPCIeFirmwareVersion(cndevPCIeFirmwareVersion_t *version, cndevDevice_t device);
/**
 * @brief Gets the UUID information. The array of UUID don't end with '\0'.
 * 
 * @param[in,out] uuidInfo The pointer to the structure storing the UUID information.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetUUID(cndevUUID_t *uuidInfo, cndevDevice_t device);
/**
 * @brief Gets the device CPU utilization.
 * 
 * @param[in,out] util The pointer to the structure storing the device CPU utilization.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetDeviceCPUUtilization(cndevDeviceCPUUtilization_t *util, cndevDevice_t device);

/**
 * @brief Gets the device CPU utilization V2.
 * 
 * @param[in,out] util The pointer to the structure storing the device CPU utilization.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetDeviceCPUUtilizationV2(cndevDeviceCPUUtilizationV2_t *util, cndevDevice_t device);
/**
 * @brief Gets the device CPU refresh time.
 * 
 * @param[in,out] time The pointer to the structure storing the device CPU refresh time.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetDeviceCPUSamplingInterval(cndevDeviceCPUSamplingInterval_t *time, cndevDevice_t device);
/**
 * @brief Sets the device CPU refresh time.
 * 
 * @param[in] time The pointer to the structure storing the device CPU refresh time.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevSetDeviceCPUSamplingInterval(cndevDeviceCPUSamplingInterval_t *time, cndevDevice_t device);
/**
 * @brief Returns the calling thread's last-error code.
 * 
 * @return
 * - The value of the last error that occurred during the execution of this program.
 */
CNDEV_EXPORT
cndevRet_t cndevGetLastError();
/**
 * @brief Gets the status of retired pages.
 *
 * Returns the list of retired pages by source, including pages that are pending retirement The
 * address information provided from this API is the hardware address of the page that was retired.
 * Note that this does not match the virtual address used in CNDev.
 *
 * @param[in,out] retirepage The pointer to the structure storing the retired pages information.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetRetiredPages(cndevRetiredPageInfo_t *retirepage, cndevDevice_t device);
/**
 * @brief Gets the status of retired pages.
 *
 * It checks for two conditions, pending page retirement and page retirement failures. If page
 * retirement is in pending, cancel the application which memory error is occurred.
 * If page retirement failure occurs, reload driver to trigger hardware repair memory error.
 *
 * @param[in,out] retirepagestatus The pointer to the structure storing the retired pages status.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetRetiredPagesStatus(cndevRetiredPageStatus_t *retirepagestatus, cndevDevice_t device);
/**
 * @brief Gets the row remapping information.
 *
 * Gets the number of rows that have been fixed for different types of memory errors. 
 * @p pendingRows in @p rows, indicates the number of rows in pending status. Reload the driver
 * will be required to actually remap the row. @p failedRows in @p rows, indicates the number of
 * failure rows. A pending remapping doesn't affect ongoing operations on the MLU because error-containment
 * and dynamic page retirement mechanism will handle it.
 *
 * @param[in,out] rows The pointer to the structure storing the remapped rows.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetRemappedRows(cndevRemappedRow_t *rows, cndevDevice_t device);
/**
 * @brief Gets the row remapping information.
 *
 * Gets the number of rows that have been fixed for different types of memory errors. 
 * @p histogram in @p rows indicates the remap availability for each bank on the MLU.
 *
 * @param[in,out] rows The pointer to the structure storing the remapped rows.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetRemappedRowsV2(cndevRemappedRowV2_t *rows, cndevDevice_t device);
/**
 * @brief Gets the repair status.
 *
 * Gets the status of memory error repair mechanism. @p isRetirePending in @p status indicates whether or
 * not there are pending page retire. Cancel the application in which memory error is
 * occured. @p isPending in @p status indicates whether or not there are pending hardware repairing.
 * Driver reload will be required to actually trigger hardware repair. @p isFailure in @p status
 * will be set if hardware repair ever failed in the past. A pending hardware repair won't affect
 * ongoing operations on the MLU because error-containment and dynamic page retirement mechanism
 * will handle it.
 *
 * @param[in,out] status The pointer to the structure storing the runtime address swap status.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetRepairStatus(cndevRepairStatus_t *status, cndevDevice_t device);
/**
 * @brief Gets the address swap information.
 *
 * @param[in,out] swaps The pointer to the structure storing the address swaps.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetAddressSwaps(cndevAddressSwap_t *swaps, cndevDevice_t device);
/**
 * @brief Gets the retired pages operation.
 *
 * @param[in,out] operation The pointer to the structure storing the retired pages operation.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetRetiredPagesOperation(cndevRetiredPageOperation_t *operation, cndevDevice_t device);

/**
 * @brief Gets device VF state.
 *
 * @param[in,out] vfstate The pointer which stores the state of VF.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetCardVfState(cndevCardVfState_t *vfstate, cndevDevice_t device);
/**
 * @brief Gets device MLU-Link port mode.
 *
 * @param[in,out] mode The pointer which stores the mode of device.
 * @param[in] device The identifier of the target device.
 * @param[in] port The count of the ports which the user selects, starting from 0.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_INVALID_LINK
 */
CNDEV_EXPORT
cndevRet_t cndevGetMLULinkPortMode(cndevMLULinkPortMode_t *mode, cndevDevice_t device, int port);
/**
 * @brief Sets device MLU-Link port mode.
 *
 * @param[in] mode The pointer which stores the mode of device.
 * @param[in] device The identifier of the target device.
 * @param[in] port The count of the ports which the user selects, starting from 0.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_INVALID_LINK
 */
CNDEV_EXPORT
cndevRet_t cndevSetMLULinkPortMode(cndevMLULinkPortMode_t *mode, cndevDevice_t device, int port);
/**
 * @brief Gets device MLU-Link port RoCE control information.
 *
 * @param[in,out] ctrl The pointer which stores RoCE control information.
 * @param[in] device The identifier of the target device.
 * @param[in] port The count of the ports which the user selects, starting from 0.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_INVALID_LINK
 */
CNDEV_EXPORT
cndevRet_t cndevGetMLULinkOverRoCECtrl(cndevMLULinkOverRoCECtrl_t *ctrl, cndevDevice_t device, int port);
/**
 * @brief Gets device port number.
 *
 * @param[in] device The identifier of the target device.
 *
 * @return
 * Returns the port count of device.
 */
CNDEV_EXPORT
int cndevGetMLULinkPortNumber(cndevDevice_t device);

/**
 * @brief Gets device tinycore utilization.
 *
 * @param[in,out] util The pointer which stores the tinycore utilization.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetTinyCoreUtilization(cndevTinyCoreUtilization_t *util, cndevDevice_t device);
/**
 * @brief Gets device arm OS memory usage information.
 *
 * @param[in,out] mem The pointer which stores arm OS memory usage.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetArmOsMemoryUsage(cndevArmOsMemoryInfo_t *mem, cndevDevice_t device);

/**
 * @brief Gets device OS memory usage information.
 *
 * @param[in,out] mem The pointer which stores device OS memory usage.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetDeviceOsMemoryUsage(cndevArmOsMemoryInfo_t *mem, cndevDevice_t device);

/**
 * @brief Gets device OS memory usage information.
 *
 * @param[out] mem The pointer which stores device OS memory usage.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetDeviceOsMemoryUsageV2(cndevDeviceOsMemoryInfo_t *mem, cndevDevice_t device);

/**
 * @brief Gets device chip ID information.
 *
 * @param[in,out] chipid The pointer which stores device chip ID.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetChipId(cndevChipId_t *chipid, cndevDevice_t device);
/**
 * @brief Gets device MLU frequency status.
 *
 * @param[in,out] status The pointer which stores MLU frequency status.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetMLUFrequencyStatus(cndevMLUFrequencyStatus_t *status, cndevDevice_t device);
/**
 * @brief Unlocks MLU frequency.
 *
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_NO_PERMISSION
 */
CNDEV_EXPORT
cndevRet_t cndevUnlockMLUFrequency(cndevDevice_t device);
/**
 * @brief Gets device MLU-Link port IP.
 *
 * @param[in,out] ip The pointer which stores device MLU-Link port IP.
 * @param[in] device The identifier of the target device.
 * @param[in] port The count of the ports which the user selects, starting from 0.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_INVALID_LINK
 */
CNDEV_EXPORT
cndevRet_t cndevGetMLULinkPortIP(cndevMLULinkPortIP_t *ip, cndevDevice_t device, int port);

/**
 * @brief Gets the information of the device's D2D CRC, only valid on MLU370.
 *
 * @param[in,out] crcInfo The pointer which stores the information of the device's D2D CRC.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetCRCInfo(cndevCRCInfo_t *crcInfo, cndevDevice_t device);

/**
 * @brief Gets the information of the device's DDR.
 *
 * @param[in,out] ddrInfo The pointer which stores the information of the device's DDR.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetDDRInfo(cndevDDRInfo_t *ddrInfo, cndevDevice_t device);

/**
 * @brief Sets the MLU Core frequency of the device.
 *
 * @param[in] setipufreq The pointer which stores the information of the user set MLU Core frequency.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT, or minIpuFreq and maxIpuFreq is not a valid frequency combination
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_NO_PERMISSION
 */
CNDEV_EXPORT
cndevRet_t cndevSetIpuFrequency(cndevSetIpuFrequency_t *setipufreq, cndevDevice_t device);
/**
 * @brief Gets the information of the device's processes utilization.
 *
 * @param[in,out] processCount The size of the space which the user allocates for storing the information of processes. At the same time, the
 * parameter The pointer which stores a pointer to the size of a space which is suit to store all information after the function ends.
 * @param[in,out] processUtil The pointer of the space which the user allocates for saving the information of processes utilization.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_INSUFFICIENT_SPACE,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetProcessUtilization(unsigned int *processCount, cndevProcessUtilization_t *processUtil, cndevDevice_t device);
/**
 * @brief Gets the list of possible MLU Core frequencies that can be used.
 *
 * @param[in,out] count The size of the space which the user allocates for storing the information of supported MLU Core frequency.
 * At the same time, the parameter The pointer which stores a pointer to the size of a space which is suit to store all information after the function ends.
 * @param[in,out] ipufreq The pointer of the space which the user allocates for saving the information of supported MLU Core frequency.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_INSUFFICIENT_SPACE,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetSupportedIpuFrequency(unsigned int *count, cndevSupportedIpuFrequency_t *ipufreq, cndevDevice_t device);

/**
 * @brief Gets the information of the device's Xid error.
 *
 * @param[in,out] xidErr The pointer which stores the information of the Xid error.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetXidError(cndevXidError_t *xidErr, cndevDevice_t device);

/**
 * @brief Translates the Xid to the corresponding message.
 *
 * @param[in] xid The Xid.
 *
 * @returns
 * - "Unknown" if the corresponding message can't be found, otherwise the corresponding message will be
 * returned.
 */
CNDEV_EXPORT
const char *cndevGetXidErrorString(cndevXidEnum_t xid);

/**
 * @brief Gets the information of the device's Xid error switch status.
 *
 * @param[in,out] xidStatus The pointer which stores the information of the Xid error switch status.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetXidStatus(cndevXidStatus_t *xidStatus, cndevDevice_t device);

/**
 * @brief Clears the information of the device's Xid error.
 *
 * @param[in,out] clearXid The pointer which stores the information of the Xid error.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_NO_PERMISSION
 */
CNDEV_EXPORT
cndevRet_t cndevClearXidError(cndevSetXid_t *clearXid, cndevDevice_t device);

/**
 * @brief Sets the information of the device's Xid error switch status.
 *
 * @param[in] setXid The pointer which stores the information of the Xid error.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_NO_PERMISSION
 */
CNDEV_EXPORT
cndevRet_t cndevSetXidStatus(cndevSetXid_t *setXid, cndevDevice_t device);

/**
 * @brief Gets the information of the device's power management limit range.
 *
 * @param[in,out] limit The pointer which stores the information of the power management limit.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetPowerManagementLimit(cndevPowerManagementLimit_t *limit, cndevDevice_t device);

/**
 * @brief Sets the information of the device's power management limit.
 *
 * @param[in,out] limitation The pointer which stores the information of the power management limit.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_NO_PERMISSION
 */
CNDEV_EXPORT
cndevRet_t cndevSetPowerManagementLimitation(cndevPowerManagementLimitation_t *limitation, cndevDevice_t device);

/**
 * @brief Gets the information of the device's power management limit.
 *
 * @param[in,out] limitation The pointer which stores the information of the power management limit.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetPowerManagementLimitation(cndevPowerManagementLimitation_t *limitation, cndevDevice_t device);

/**
 * @brief Gets the information of the device's power management default limit.
 *
 * @param[in,out] limitation The pointer which stores the information of the power management default limit.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetPowerManagementDefaultLimitation(cndevPowerManagementLimitation_t *defaultLimitation, cndevDevice_t device);

/**
 * @brief Gets the information of the device's power management limit range.
 *
 * @param[in,out] limit The pointer which stores the information of the power management limit range.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetPowerManagementLimitationRange(cndevPowerManagementLimitationRange_t *limit, cndevDevice_t device);

/**
 * @brief Gets the information of the device's max performance information.
 *
 * @param[in,out] maxperformance The pointer which stores the information of the max performance.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetDeviceMaxPerformance(cndevDeviceMaxPerformance_t *maxperformance, cndevDevice_t device);

/**
 * @brief Gets the information of the device's overtemperature.
 *
 * @param[in,out] tempInfo The pointer which stores the information of overtemperature.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetOverTemperatureInfo(cndevOverTemperatureInfo_t *tempInfo, cndevDevice_t device);

/**
 * @brief Gets the information of the device's performance throttle reason.
 * When performance exceeds the maximum limit, the device will throttle performance, could call this API to get
 * what are reasons for throttle performance.
 *
 * @param[in,out] reason The pointer which stores the information of performance throttle reason.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetPerformanceThrottleReason(cndevPerformanceThrottleReason_t *reason, cndevDevice_t device);

/**
 * @brief Gets the information of the device's compute mode.
 *
 * @param[in,out] mode The pointer which stores the information of compute mode.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetComputeMode(cndevComputeMode_t *mode, cndevDevice_t device);

/**
 * @brief Sets the information of the device's compute mode.
 *
 * @param[in] mode The pointer which stores the information of compute mode.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_NO_PERMISSION
 */
CNDEV_EXPORT
cndevRet_t cndevSetComputeMode(cndevComputeMode_t *mode, cndevDevice_t device);

/**
 * @brief Gets the information of the device's PCIe replay count.
 *
 * @param[in,out] count The pointer which stores the information of PCIe replay count.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetPcieReplayCounter(cndevPcieReplayCounter_t *count, cndevDevice_t device);

/**
 * @brief Gets the information of the device's SR-IOV mode.
 *
 * @param[in,out] mode The pointer which stores the information of SR-IOV mode.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetSRIOVMode(cndevSRIOVMode_t *mode, cndevDevice_t device);
/**
 * @brief Sets the information of the device's SR-IOV mode.
 *
 * @param[in] mode The pointer which stores the information of SR-IOV mode.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_NO_PERMISSION,
 * - ::CNDEV_ERROR_IN_USE
 */
CNDEV_EXPORT
cndevRet_t cndevSetSRIOVMode(cndevSRIOVMode_t *mode, cndevDevice_t device);

/**
 * @brief Creates an MLU instance by profile ID base on MIM.
 *
 * @param[out] miHandle The pointer which stores the MLU instance handle.
 * @param[in] profileId The MLU instance profile ID. For more information, see ::cndevGetMluInstanceProfileInfo.
 * @param[in] device The identifier of the target device.
 * @param[in] instanceName The name of the instance created.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INSUFFICIENT_SPACE,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_NO_PERMISSION,
 * - ::CNDEV_ERROR_NOT_FOUND,
 * - ::CNDEV_ERROR_DUPLICATE
 */
CNDEV_EXPORT
cndevRet_t cndevCreateMluInstanceByProfileId(cndevMluInstance_t *miHandle, unsigned int profileId, cndevDevice_t device, char *instanceName);

/**
 * @brief Creates an MLU instance by profile name base on MIM.
 *
 * @param[out] miHandle The pointer which stores the MLU instance handle.
 * @param[in] profileName The MLU instance profile Name. For more information, see ::cndevGetMluInstanceProfileInfo.
 * @param[in] device The identifier of the target device.
 * @param[in] instanceName The name of the instance created.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INSUFFICIENT_SPACE,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_NO_PERMISSION,
 * - ::CNDEV_ERROR_NOT_FOUND, 
 * - ::CNDEV_ERROR_DUPLICATE
 */
CNDEV_EXPORT
cndevRet_t cndevCreateMluInstanceByProfileName(cndevMluInstance_t *miHandle, char *profileName, cndevDevice_t device, char *instanceName);

/**
 * @brief Creates MLU instance with the specified placement by profile ID base on MIM.
 *
 * @param[in] placement The requested placement.
 * @param[out] miHandle The pointer which stores the MLU instance handle.
 * @param[in] profileId The MLU instance profile ID. For more information, see ::cndevGetMluInstanceProfileInfo.
 * @param[in] device The identifier of the target device.
 * @param[in] instanceName The name of the instance created.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INSUFFICIENT_SPACE,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_NO_PERMISSION,
 * - ::CNDEV_ERROR_NOT_FOUND,
 * - ::CNDEV_ERROR_DUPLICATE
 */
CNDEV_EXPORT
cndevRet_t cndevCreateMluInstanceByProfileIdWithPlacement(cndevMluInstancePlacement_t *placement,
                                               cndevMluInstance_t *miHandle, unsigned int profileId, cndevDevice_t device, char *instanceName);

/**
 * @brief Creates an MLU instance with the specified placement by profilename base on MIM.
 *
 * @param[in] placement The requested placement.
 * @param[out] miHandle The pointer which stores the MLU instance handle.
 * @param[in] profileName The MLU instance profile Name. For more information, see ::cndevGetMluInstanceProfileInfo.
 * @param[in] device The identifier of the target device.
 * @param[in] instanceName The name of the instance created.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INSUFFICIENT_SPACE,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_NO_PERMISSION,
 * - ::CNDEV_ERROR_NOT_FOUND,
 * - ::CNDEV_ERROR_DUPLICATE
 */
CNDEV_EXPORT
cndevRet_t cndevCreateMluInstanceByProfileNameWithPlacement(cndevMluInstancePlacement_t *placement,
                                               cndevMluInstance_t *miHandle, char *profileName, cndevDevice_t device, char *instanceName);
/**
 * @brief Destroys an MLU instance by MLU instance handle base on MIM.
 *
 * @param[in] miHandle The MLU instance handle.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_NO_PERMISSION,
 * - ::CNDEV_ERROR_NOT_FOUND,
 * - ::CNDEV_ERROR_IN_USE
 */
CNDEV_EXPORT
cndevRet_t cndevDestroyMluInstanceByHandle(cndevMluInstance_t miHandle);

/**
 * @brief Destroys an MLU instance by MLU instance name base on MIM.
 *
 * @param[in] device The identifier of the target device.
 * @param[in] instanceName The name of the instance you want to destroy.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_NO_PERMISSION,
 * - ::CNDEV_ERROR_NOT_FOUND,
 * - ::CNDEV_ERROR_IN_USE
 */
CNDEV_EXPORT
cndevRet_t cndevDestroyMluInstanceByInstanceName(cndevDevice_t device, char *instanceName);
/**
 * @brief Gets MLU instance profile information base on MIM.
 *
 * @param[in,out] profileInfo The pointer which stores the information of an MLU instance profile.
 * @param[in] profile The MLU instance profile ID.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_NOT_FOUND
 */
CNDEV_EXPORT
cndevRet_t cndevGetMluInstanceProfileInfo(cndevMluInstanceProfileInfo_t * profileInfo, int profile, cndevDevice_t device);

/**
 * @brief Gets MLU instance possible placements base on MIM.
 *
 * @param[in,out] count The size of the space which the user allocates for storing the information of placements. At the same time,
 *   the parameter The pointer which stores a pointer to the size of a space which is suit to store all information after the function ends.
 * @param[in,out] placement The pointer of the space which the user allocates for saving the information of placements.
 * @param[in] profileId The MLU instance profile ID. For more information, see ::cndevGetMluInstanceProfileInfo.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_NOT_FOUND
 */
CNDEV_EXPORT
cndevRet_t cndevGetMluInstancePossiblePlacements(int *count, cndevMluInstancePlacement_t *placement, int profileId, cndevDevice_t device);

/**
 * @brief Gets MLU instance remaining capacity for the given profile ID base on MIM.
 *
 * @param[in,out] count Returns remaining instance count for the profile ID.
 * @param[in] profileId The MLU instance profile ID. For more information, see ::cndevGetMluInstanceProfileInfo.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_NOT_FOUND
 */
CNDEV_EXPORT
cndevRet_t cndevGetMluInstanceRemainingCapacity(int *count, int profileId, cndevDevice_t device);

/**
 * @brief Gets MLU instance max count for the given profile ID base on MIM.
 *
 * @param[in,out] count Returns max instance count for the profile ID.
 * @param[in] profileId The MLU instance profile ID. For more information, see ::cndevGetMluInstanceProfileInfo.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_NOT_FOUND
 */
CNDEV_EXPORT
cndevRet_t cndevGetMaxMluInstanceCount(int *count, int profileId, cndevDevice_t device);

/**
 * @brief Gets MLU instance information base on MIM.
 *
 * @param[in,out] miInfo The pointer which stores the information of an MLU instance.
 * @param[in] miHandle The MLU instance handle.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 * - ::CNDEV_ERROR_NOT_FOUND
 */
CNDEV_EXPORT
cndevRet_t cndevGetMluInstanceInfo(cndevMluInstanceInfo_t *miInfo, cndevMluInstance_t miHandle);

/**
 * @brief Gets all MLU instance information base on MIM
 *
 * @param[in,out] count The size of the space which the user allocates for storing the information of an MLU instance. At the same time,
 *   the parameter The pointer which stores a pointer to the size of a space which is suit to store all information after the function ends.
 * @param[in,out] miInfo The pointer of the space which the user allocates for saving the information of an MLU instance.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_NO_PERMISSION
 */
CNDEV_EXPORT
cndevRet_t cndevGetAllMluInstanceInfo(int *count, cndevMluInstanceInfo_t *miInfo, cndevDevice_t device);

/**
 * @brief Gets MLU instance handle for the given index under its parent device base on MIM.
 *
 * @param[out] miHandle The MLU instance handle.
 * @param[in] index The index of the MLU instance.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_FOUND
 */
CNDEV_EXPORT
cndevRet_t cndevGetMluInstanceByIndex(cndevMluInstance_t * miHandle, int index, cndevDevice_t device);

/**
 * @brief Gets parent device handle from an MLU instance handle.
 *
 * @param[out] devcieHanlde The device handle.
 * @param[in] miHandle The MLU instance handle.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetDeviceHandleFromMluInstanceHandle(cndevDevice_t* devcieHanlde, cndevMluInstance_t miHandle);

/**
 * @brief Gets MLU instance for given profile ID base on MIM.
 *
 * @param[in] miHandle The MLU instance handle.
 * @param[in,out] count The size of the space which the user allocates for storing the information of MLU instance. At the same time,
 *   the parameter The pointer which stores a pointer to the size of a space which is suit to store all information after the function ends.
 * @param[in] profileId The MLU instance profile ID. For more information, see ::cndevGetMluInstanceProfileInfo.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_NO_PERMISSION
 */
CNDEV_EXPORT
cndevRet_t cndevGetMluInstance(cndevMluInstance_t* miHandle,  int *count, int profileId,  cndevDevice_t device);
/**
 * @brief Gets the MLU instance ID for the given MLU device handle base on MIM.
 *
 * @param[out] instanceId The MLU instance ID.
 * @param[in] miHandle The MLU instance handle.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetMluInstanceId(int * instanceId, cndevMluInstance_t miHandle);
/**
 * @brief Gets the MLU instance for the given MLU instance ID base on MIM.
 *
 * @param[out] miHandle The MLU instance handle.
 * @param[in] instanceId The MLU instance ID.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetMluInstanceById(cndevMluInstance_t * miHandle, int instanceId, cndevDevice_t device);
/**
 * @brief Gets MLU device handle for the given index base on MIM.
 *
 * @param[in] index The index of the target MLU.
 * @param[out] handle The MLU device handle.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_NOT_FOUND
 */
CNDEV_EXPORT
cndevRet_t cndevGetDeviceHandleByIndex(int index, cndevDevice_t *handle);
/**
 * @brief Gets the MLU device handle for the given PCI bus ID.
 *
 * @param[in] pciBusId The PCI bus ID of the target MLU.
 * @param[out] handle The MLU device handle.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_NOT_FOUND
 */
CNDEV_EXPORT
cndevRet_t cndevGetDeviceHandleByPciBusId(const char *pciBusId, cndevDevice_t *handle);
/**
 * @brief Gets the MLU device handle for the given serial number.
 *
 * @param[in] sn The serial number of the target MLU.
 * @param[out] handle The MLU device handle.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_NOT_FOUND
 */
CNDEV_EXPORT
cndevRet_t cndevGetDeviceHandleBySerial(const char *sn, cndevDevice_t *handle);
/**
 * @brief Gets the MLU device handle for the given UUID.
 *
 * @param[in] uuid The UUID of the target MLU.
 * @param[out] handle The MLU device handle.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_NOT_FOUND
 */
CNDEV_EXPORT
cndevRet_t cndevGetDeviceHandleByUUID(const char *uuid, cndevDevice_t *handle);
/**
 * @brief Gets the parity error information.
 *
 * @param[in,out] error The pointer to the structure storing the information of parity error.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetParityError(cndevParityError_t *error, cndevDevice_t handle);

/**
 * @brief Gets the information of device power consumption.
 *
 * @param[in,out] power The pointer which stores the information of the device power consumption after the
 * function ends.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetDevicePower(cndevPowerInfo_t *power, cndevDevice_t device);

/**
 * @brief Gets the information of the device temperature.
 *
 * @param[in,out] temp The pointer which stores the information of the device temperature after the function
 * ends.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetDeviceTemperature(cndevTemperatureInfo_t *temp, cndevDevice_t device);

/**
 * @brief Gets the information of the device's MIM mode.
 *
 * @param[in,out] mode The pointer which stores the information of MIM mode.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetMimMode(cndevMimMode_t *mode, cndevDevice_t device);
/**
 * @brief Sets the information of the device's MIM mode.
 *
 * @param[in] mode The pointer which stores the information of MIM mode.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_NO_PERMISSION
 */
CNDEV_EXPORT
cndevRet_t cndevSetMimMode(cndevMimMode_t *mode, cndevDevice_t device);
/**
 * @brief Gets the information of the device's docker param.
 *
 * @param[in,out] param The pointer which stores the information of docker param.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_USE
 */
CNDEV_EXPORT
cndevRet_t cndevGetDockerParam(cndevDockerParam_t *param, cndevDevice_t device);

/**
 * @brief Gets the information of the device's sMLU mode.
 *
 * @param[in,out] mode The pointer which stores the information of sMLU mode.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetSMLUMode(cndevSMLUMode_t *mode, cndevDevice_t device);
/**
 * @brief Sets the information of the device's sMLU mode.
 *
 * @param[in] mode The pointer which stores the information of sMLU mode.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_NO_PERMISSION
 */
CNDEV_EXPORT
cndevRet_t cndevSetSMLUMode(cndevSMLUMode_t *mode, cndevDevice_t device);
/**
 * @brief Gets MLU instance profile ID count based on sMLU.
 *
 * @param[in,out] profileId The pointer which stores the ID information of MLU instance profile ID.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetSMluProfileIdInfo(cndevSMluProfileIdInfo_t *profileId, cndevDevice_t device);
/**
 * @brief Gets MLU instance profile information based on sMLU.
 *
 * @param[in,out] profileInfo The pointer which stores the information of MLU instance profile.
 * @param[in] profile The profile ID.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetSMluProfileInfo(cndevSMluProfileInfo_t *profileInfo, int profile, cndevDevice_t device);

/**
 * @brief Creates an MLU instance by profile ID based on sMLU.
 *
 * @param[out] miHandle The MLU instance handle.
 * @param[in] profileId The MLU instance profile ID. For more information, see cndevGetSMluProfileIdInfo.
 * @param[in] device The identifier of the target device.
 * @param[in] instanceName The name of the instance you want to create.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_NO_PERMISSION,
 * - ::CNDEV_ERROR_INSUFFICIENT_SPACE,
 * - ::CNDEV_ERROR_DUPLICATE
 */
CNDEV_EXPORT
cndevRet_t cndevCreateSMluInstanceByProfileId(cndevMluInstance_t *miHandle, unsigned int profileId, cndevDevice_t device, char * instanceName);


/**
 * @brief Creates an MLU instance by profile name based on sMLU.
 *
 * @param[out] miHandle The MLU instance handle.
 * @param[in] profileName The MLU instance profile Name. For more information, see cndevGetSMluProfileIDInfo.
 * @param[in] device The identifier of the target device.
 * @param[in] instanceName The name of the instance you want to create.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_NO_PERMISSION,
 * - ::CNDEV_ERROR_INSUFFICIENT_SPACE,
 * - ::CNDEV_ERROR_DUPLICATE
 */
CNDEV_EXPORT
cndevRet_t cndevCreateSMluInstanceByProfileName(cndevMluInstance_t *miHandle, char * profileName, cndevDevice_t device, char * instanceName);

/**
 * @brief Destroys an MLU instance by MLU instance Handle based on sMLU.
 *
 * @param[in] miHandle The MLU instance handle.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_NO_PERMISSION,
 * - ::CNDEV_ERROR_IN_USE
 */
CNDEV_EXPORT
cndevRet_t cndevDestroySMluInstanceByHandle(cndevMluInstance_t miHandle);

/**
 * @brief Destroys an MLU instance by MLU instance name based on sMLU.
 *
 * @param[in] device The identifier of the target device.
 * @param[in] instanceName The MLU instance name.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_NO_PERMISSION,
 * - ::CNDEV_ERROR_IN_USE
 */
CNDEV_EXPORT
cndevRet_t cndevDestroySMluInstanceByInstanceName(cndevDevice_t device, char *instanceName);

/**
 * @brief Gets MLU instance information based on sMLU.
 *
 * @param[in,out] miInfo The pointer which stores the information of an MLU instance.
 * @param[in] miHandle The MLU instance handle.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetSMluInstanceInfo(cndevSMluInfo_t *miInfo, cndevMluInstance_t miHandle);


/**
 * @brief Updates the quota of MLU instance by MLU instance Handle based on sMLU.
 *
 * @param[in] miHandle The MLU instance handle.
 * @param[in] instance_quota   The pointer which stores the information of an MLU instance.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_INSUFFICIENT_SPACE,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_NO_PERMISSION,
 * - ::CNDEV_ERROR_NOT_FOUND
 */
CNDEV_EXPORT
cndevRet_t cndevUpdateSMluInstanceQuotaByHandle(cndevMluInstance_t miHandle, cndevSMluSet_t *instance_quota);

/**
 * @brief Updates the quota of an MLU instance by MLU instance name based on sMLU.
 *
 * @param[in] device The identifier of the target device.
 * @param[in] instanceName The MLU instance name.
 * @param[in] instance_quota   The pointer which stores the information of an MLU instance.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_INSUFFICIENT_SPACE,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_NO_PERMISSION,
 * - ::CNDEV_ERROR_NOT_FOUND
 */
CNDEV_EXPORT
cndevRet_t cndevUpdateSMluInstanceQuotaByInstanceName(cndevDevice_t device, char * instanceName, cndevSMluSet_t *instance_quota);

/**
 * @brief Gets all MLU instance information based on sMLU.
 *
 * @param[in,out] count The size of the space which the user allocates for storing the information of an MLU instance. At the same time,
 *   the parameter The pointer which stores a pointer to the size of a space which is suit to store all information after the function ends.
 * @param[in,out] miInfo The pointer of the space which the user allocates for saving the information of an MLU instance.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_NOT_FOUND,
 * - ::CNDEV_ERROR_NO_PERMISSION,
 * - ::CNDEV_ERROR_INSUFFICIENT_SPACE
 */
CNDEV_EXPORT
cndevRet_t cndevGetAllSMluInstanceInfo(int *count, cndevSMluInfo_t *miInfo, cndevDevice_t device);

/**
 * @brief Creates sMLU profile based on sMLU.
 *
 * @param[in,out] profileInfo The pointer which stores the information of MLU instance profile.
 * @param[out] profileId The MLU instance profile ID.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_NO_PERMISSION
 */
CNDEV_EXPORT
cndevRet_t cndevCreateSMluProfileInfo(cndevSMluSet_t *profileInfo, int *profileId, cndevDevice_t device);

/**
 * @brief Destroys sMLU profile based on sMLU.
 *
 * @param[in] profileId The MLU instance profile ID.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_NO_PERMISSION,
 * - ::CNDEV_ERROR_DUPLICATE
 */
CNDEV_EXPORT
cndevRet_t cndevDestroySMluProfileInfo(int profileId, cndevDevice_t device);
/**
 * @brief Gets the information of the device's Xid error.
 *
 * @param[in,out] xidErr The pointer which stores the information of the Xid error.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_NO_PERMISSION,
 * - ::CNDEV_ERROR_IN_USE
 */
CNDEV_EXPORT cndevRet_t cndevGetXidErrorV2(cndevXidErrorV2_t *xidErr, cndevDevice_t device);
/**
 * @brief Injects an Xid error.
 *
 * @param[in] xidEnum The Xid error to be injected.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS
 */
CNDEV_EXPORT cndevRet_t cndevInjectXidError(cndevXidEnum_t xidEnum, cndevDevice_t device);
/**
 * @brief Configs device configs and it will be effective after call ::cndevDeviceActiveConfigs().
 *
 * @param[in] value The value which will be config to MLU Driver.
 * @param[in] type The type of device configs.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_NO_PERMISSION
 * 
 * @note
 * - If this API fails, it is recommended to call ::cndevDeviceResetConfigs() to reset the configuration and prevent the next call to this API from failing.
 */
CNDEV_EXPORT
cndevRet_t cndevDeviceSetConfigs(unsigned long long value, cndevDeviceConfigsTypeEnum_t type, cndevDevice_t device);
/**
 * @brief Queries configs value that has been activated in MLU Driver.
 *
 * @param[out] value The pointer which stores device configs value.
 * @param[in] type The type of device configs.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_NO_PERMISSION
 */
CNDEV_EXPORT
cndevRet_t cndevDeviceGetConfigs(unsigned long long *value, cndevDeviceConfigsTypeEnum_t type, cndevDevice_t device);
/**
 * @brief Actives configs which has been configured by ::cndevDeviceSetConfigs(), please ensure no one is using the device.
 *
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_NO_PERMISSION,
 * - ::CNDEV_ERROR_IN_USE
 * 
 *  * @note
 * - If this API fails, it is recommended to call ::cndevDeviceResetConfigs() to reset the configuration and prevent the next call to this API from failing.
 */
CNDEV_EXPORT
cndevRet_t cndevDeviceActiveConfigs(cndevDevice_t device);
/**
 * @brief Resets device configs to default value. Ensure that no one is using the device.
 *
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_NO_PERMISSION,
 * - ::CNDEV_ERROR_IN_USE
 */
CNDEV_EXPORT
cndevRet_t cndevDeviceResetConfigs(cndevDevice_t device);
/**
 * @brief Gets total available and used size of BAR4 memory. BAR4 is used to map the device memory.
 *
 * @param[in,out] bar4Memory The pointer which stores the information of the device's BAR4 memory after the function ends.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetBAR4MemoryInfo(cndevBAR4Memory_t *bar4Memory, cndevDevice_t device);
/**
 * @brief Gets major and minor compute capability of the device.
 *
 * @param[in,out] major The pointer which stores the major compute capability of the device.
 * @param[in,out] minor The pointer which stores the minor compute capability of the device.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetComputeCapability(unsigned int *major, unsigned int *minor, cndevDevice_t device);
/**
 * @brief Gets the information of the device's application clock.
 *
 * @param[in] clockType Identify which clock domain to query.
 * @param[in,out] clockMHz The pointer which stores the current setting of application clock in MHz, 0 indicates it has not been set.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetApplicationsClock(cndevClockType_t clockType, unsigned int *clockMHz, cndevDevice_t device);



typedef union  {
    /**
    * Double value.*/
    double              dVal;
    /**
    * Unsigned int value.*/
    unsigned int        uiVal;
    /**
    * Unsigned long value.*/
    unsigned long       ulVal;
    /**
    * Unsigned long long value.*/
    unsigned long long  ullVal;
    /**
    * Signed int value.*/
    int                 siVal;
    /**
    * Signed long value.*/
    long                slVal;
    /**
    * Signed long long value.*/
    long long           sllVal;
} cndevValue_t;
typedef enum {
    /**
    * Retrieved value type is double.*/
    CNDEV_VALUE_TYPE_DOUBLE = 0,
    /**
    * Retrieved value type is unsigned int.*/
    CNDEV_VALUE_TYPE_UNSIGNED_INT = 1,
    /**
    * Retrieved value type is unsigned long.*/
    CNDEV_VALUE_TYPE_UNSIGNED_LONG = 2,
    /**
    * Retrieved value type is unsigned long long.*/
    CNDEV_VALUE_TYPE_UNSIGNED_LONG_LONG = 3,
    /**
    * Retrieved value type is int.*/
    CNDEV_VALUE_TYPE_INT = 4,
    /**
    * Retrieved value type is long.*/
    CNDEV_VALUE_TYPE_LONG = 5,
    /**
    * Retrieved value type is long long.*/
    CNDEV_VALUE_TYPE_LONG_LONG = 6,
} cndevFieldVaulesTypeNum_t;
typedef enum {
    /**
    * Current ECC mode.*/
    cndevFieldECCCurrent = 0,
    /**
    * Pending ECC mode.*/
    cndevFieldECCPending = 1,
    /**
    * SRAM ECC SBE count after loading MLU Driver.*/
    cndevFieldSramECCSBEVolCount = 2,
    /**
    * SRAM ECC DBE count after loading MLU Driver.*/
    cndevFieldSramECCDBEVolCount = 3,
    /**
    * DRAM ECC SBE count after loading MLU Driver.*/
    cndevFieldDramECCSBEVolCount = 4,
    /**
    * DRAM ECC DBE count after loading MLU Driver.*/
    cndevFieldDramECCDBEVolCount = 5,
    /**
    * Current PCIe generation.*/
    cndevFieldCurrentPciSpeed = 6,
    /**
    * Current PCIe link width.*/
    cndevFieldCurrentPciWidth = 7,
    /**
    * Device port number.*/
    cndevFieldMLULinkPortNum = 8,
    /**
    * Status of the MLU-Link. 0 represents the status is inactive, and 1 represents it is active.*/
    cndevFieldMLULinkStatus = 9,
    /**
    * The retired page count due to SBE.*/
    cndevFieldRetiredPagesSBE = 10,
    /**
    * The retired page count due to DBE.*/
    cndevFieldRetiredPagesDBE = 11,
    /**
    * The pending status of the retired page. 0 represents the page is not pending, and 1 represents it is currently pending.*/
    cndevFieldRetiredPagesPending = 12,
    /**
    * The count of corrected rows.*/
    cndevFieldRemapedRowCorrect = 13,
    /**
    * The count of uncorrected rows.*/
    cndevFieldRemapedRowUnCorrect = 14,
    /**
    * The count of rows that are pending remapping.*/
    cndevFieldRemapedRowPending = 15,
    /**
    * The count of rows that failed to be remapped.*/
    cndevFieldRemapedRowFailure = 16,
    /**
    * Current power, unit: W.*/
    cndevFieldPowerCurrent = 17,
    /**
    * Capping power, unit: W.*/
    cndevFieldPowerCapping = 18,
    /**
    * Minimum power limit, unit: W.*/
    cndevFieldPowerCappingMinLimit = 19,
    /**
    * Maximum power limit, unit: W.*/
    cndevFieldPowerCappingMaxLimit = 20,
    /**
    * Default power limit, unit: W.*/
    cndevFieldPowerCappingDefaultLimit = 21,
    /**
    * MLU device physical total memory, unit: MB.*/
    cndevFieldPhysicalMemoryTotal = 22,
    /**
    * MLU device physical used memory, unit: MB.*/
    cndevFieldPhysicalMemoryUsed = 23,
    /**
    * MLU device virtual total memory, unit: MB.*/
    cndevFieldVirtualMemoryTotal = 24,
    /**
    * MLU device virtual used memory, unit: MB.*/
    cndevFieldVirtualMemoryUsed = 25,
    /**
    * MLU DDR data width.*/
    cndevFieldDDRDataWidth = 26,
    /**
    * MLU DDR band width, unit: GB/s.*/
    cndevFieldDDRBandWidth = 27,
    /**
    * The compute mode of device. 0 represents default mode, and 1 represents exclusive mode.*/
    cndevFieldComputeMode = 28,
    /**
    * The count of PCIe replays.*/
    cndevFieldPcieReplay = 29,
    /**
    * Over-temperature power-off temperature, unit: ℃.*/
    cndevFieldOverTemperaturePowerOffTemp = 30,
    /**
    * Over-temperature power-off times.*/
    cndevFieldOverTemperaturePowerOffTimes = 31,
    /**
    * Over-temperature under clocking temperature, unit: ℃.*/
    cndevFieldOverTemperatureUnderClockTemp = 32,
    /**
    * Over-temperature under clocking temperature times, unit: ℃.*/
    cndevFieldOverTemperatureUnderClockTimes = 33,
    /**
    * Board temperature, unit: ℃.*/
    cndevFieldTemperatureBoard = 34,
    /**
    * Cluster temperature, unit: ℃.*/
    cndevFieldTemperatureCluster = 35,
    /**
    * Memory die temperature, unit: ℃.*/
    cndevFieldTemperatureMemoryDie = 36,
    /**
    * Chip temperature, unit: ℃.*/
    cndevFieldTemperatureChip = 37,
    /**
    * Air inlet temperature, unit: ℃.*/
    cndevFieldTemperatureAirIn = 38,
    /**
    * Air outlet temperature, unit: ℃.*/
    cndevFieldTemperatureAirOut = 39,
    /**
    * Memory temperature, unit: ℃.*/
    cndevFieldTemperatureMemory = 40,
    /**
    * Video input temperature, unit: ℃.*/
    cndevFieldTemperatureVI = 41,
    /**
    * Device CPU temperature, unit: ℃.*/
    cndevFieldTemperatureCPU = 42,
    /**
    * ISP temperature, unit: ℃.*/
    cndevFieldTemperatureISP = 43,
    /**
    * MLU average core utilization.*/
    cndevFieldCoreAverageUtilization = 44,
    /**
    * MLU core utilization.*/
    cndevFieldCoreUtilization = 45,
    /**
    * Video encoder count.*/
    cndevFieldEncoderCount = 46,
    /**
    * Video encoder utilization.*/
    cndevFieldEncoderUtilization = 47,
    /**
    * Video decoder count.*/
    cndevFieldDecoderCount = 48,
    /**
    * Video decoder utilization.*/
    cndevFieldDecoderUtilization = 49,
    /**
    * Image codec count.*/
    cndevFieldImageCodecCount = 50,
    /**
    * Image codec utilization.*/
    cndevFieldImageCodecUtilization = 51,
    /**
    * Device slowdown temperature.*/
    cndevFieldTemperatureSlowDown = 52,
    /**
    * Device shutdown temperature.*/
    cndevFieldTemperatureShutDown = 53,
    /**
    * MLU device reserved memory, unit: MB.*/
    cndevFieldReservedMemory = 54,
    /**
    * MLU device global memory, unit: MB.*/
    cndevFieldGlobalMemory = 55,
    /**
    * DRAM ECC SBE count after manufacturing date.*/
    cndevFieldDramECCSBEAggregateCount = 56,
    /**
    * DRAM ECC DBE count after manufacturing date.*/
    cndevFieldDramECCDBEAggregateCount = 57,
    /**
    * SRAM ECC SBE count after manufacturing date.*/
    cndevFieldSramECCSBEAggregateCount = 58,
    /**
    * SRAM ECC DBE count after manufacturing date.*/
    cndevFieldSramECCDBEAggregateCount = 59,
    /**
    * SRAM ECC PARITY count after manufacturing date.*/
    cndevFieldSramECCParityAggregateCount = 60,
    /**
    * SRAM ECC PARITY count after loading MLU Driver.*/
    cndevFieldSramECCParityVolCount = 61,
    /**
     * The max ID of Field.*/
    cndevFieldNum = 200,
} cndevFieldIdNum_t;
typedef struct {
    cndevFieldIdNum_t fieldId;            /**< ID of the CNDev field to retrieve. It must be set before any function call the uses this struct.*/
    cndevRet_t ret;                       /**< Return code for retrieving this field.*/
    cndevValue_t value;                   /**< Value for this field. It is only valid when @p ret is ::CNDEV_SUCCESS.*/
    cndevFieldVaulesTypeNum_t valueType;  /**< Type of the value stored in value. It is only valid when @p ret is ::CNDEV_SUCCESS.*/
    long long latencyUsec;                /**< The time taken to update this field value (in microseconds). It is only valid when @p ret is ::CNDEV_SUCCESS.*/
    long long timestamp;                  /**< CPU timestamp of this value since 1970 (in microseconds). It is only valid when @p ret is ::CNDEV_SUCCESS.*/
    unsigned int scopeId;                 /**< @p scopeId represents:
                                               - Link ID when used for getting MLU-Link status.
                                               - Core ID when used for getting MLU core utilization.
                                               - Memory die ID when used for getting memory die temperature.
                                               - Cluster ID when used for getting cluster temperature.*/
} cndevFieldVaule_t;

/**
 * @brief Gets value of the given field for device.
 * This API supports the retrieval of multiple fields in a single request.
 *
 * @param[in] device The identifier of the target device.
 * @param[in] valueCount The count of fields that are needed to get value.
 * @param[in,out] value The pointer which stores the value of the information of given fields.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevDeviceGetFieldValues(cndevDevice_t device, int valueCount, cndevFieldVaule_t * value);

/**
 * @brief Creates an event handle.
 *
 * @param[out] handle The created event handle.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_MALLOC,
 * - ::CNDEV_ERROR_UNKNOWN
 */
CNDEV_EXPORT
cndevRet_t cndevEventHandleCreate(cndevEventHandle *handle);

/**
 * @brief Destroys the event handle.
 *
 * @param[in] handle The pointer of event handle to destroy.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 */

CNDEV_EXPORT
cndevRet_t cndevEventHandleDestroy(cndevEventHandle handle);

/**
 * @brief Registers the given event type to device.
 *
 * @param[in] handle     The event handle.
 * @param[in] eventTypes The event type for which registration is desired.
 * @param[in] device     The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevRegisterEvents(cndevEventHandle handle, unsigned long long eventTypes, cndevDevice_t device);

/**
 * @brief Waits for the event to occur. The operation is not thread safe.
 *
 * @param[in]  handle     The event handle.
 * @param[out] eventData  The obtained event information.
 * @param[in]  timeout    The maximum amount of waiting time in milliseconds.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevEventWait(cndevEventHandle handle, cndevEventData_t * eventData, int timeout);

/**
 * @brief Gets the supported event type.
 *
 * @param[out] eventTypes  The supported event type.
 * @param[in]  device      The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetSupportedEventTypes(unsigned long long * eventTypes, cndevDevice_t device);

/**
 * @brief Injects an event Xid error to device for simulating a real event.
 *
 * @param[in]  eventError  The event Xid error to be injected.
 * @param[in]  device      The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevInjectEventError(__uint64_t eventError, cndevDevice_t device);

/**
 * @brief Gets the device voltage information.
 * If a certain voltage value is -1, it means that obtaining this voltage is not supported on the device.
 *
 * @param[out] voltage  The pointer which stores the voltage information.
 * @param[in]  device   The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetVoltageInfo(cndevVoltageInfo_t *voltage, cndevDevice_t device);

/**
 * @brief Gets the device current information.
 * If a certain current value is -1, it means that obtaining this current is not supported on the device.
 *
 * @param[out] current  The pointer which stores the current information.
 * @param[in]  device   The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_NOT_SUPPORTED
 */
CNDEV_EXPORT
cndevRet_t cndevGetCurrentInfo(cndevCurrentInfo_t * current, cndevDevice_t device);

/**
 * @brief Resets device to its default status. Ensure that no one is using the device.
 * The maximum waiting time is 120 seconds, reaching this value will return ::CNDEV_ERROR_TIMEOUT.
 *
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_NO_PERMISSION,
 * - ::CNDEV_ERROR_IN_USE,
 * - ::CNDEV_ERROR_TIMEOUT
 */
CNDEV_EXPORT
cndevRet_t cndevResetDevice(cndevDevice_t device);

#if defined(__cplusplus)
}
#endif  /*__cplusplus*/
#endif  // INCLUDE_CNDEV_H_
