#ifndef INCLUDE_CNDEV_H_
#define INCLUDE_CNDEV_H_

#ifndef WIN32
#include <libgen.h>
#include <linux/pci_regs.h>
#include <unistd.h>
#include <netinet/in.h>
#else
#include <winsock2.h>
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
typedef int __int32_t;                /**< Defines \p __int32_t as ``int``.*/
typedef unsigned int __uint32_t;      /**< Defines \p __uint32_t as ``unsigned int``.*/
typedef unsigned long __uint64_t;     /**< Defines \p __uint64_t as ``unsigned long``.*/
typedef long __int64_t;               /**< Defines \p __int64_t as ``long``.*/

/**
 * @brief The return values for CNDev APIs.
 */
typedef enum {

	CNDEV_SUCCESS = 0,
	/**<
	 * This indicates that the API has been executed successfully.
	 */
	CNDEV_ERROR_NO_DRIVER = 1,
	/**<
	 * This indicates that the MLU driver was not loaded or failed to load. You need to reload the driver.
	 * This error is usually returned by the ::cndevInit call. Make sure
	 * the driver is successfully installed.
	 */
	CNDEV_ERROR_LOW_DRIVER_VERSION = 2,
	/**<
	 * This indicates that the version of MLU driver is below the minimum supported version.
	 * Upgrade the driver to the minimum supported version, which can be obtained by calling
	 * ::cndevGetLowestSupportDriverVersion.
	 */
	CNDEV_ERROR_UNSUPPORTED_API_VERSION = 3,
	/**<
	 * This indicates an unsupported or invalid version for this API. Use the correct version number for the API.
	 * Refer to CNDEV_VERSION_<X> for information about version number.
	 */
	CNDEV_ERROR_UNINITIALIZED = 4,
	/**<
	 * This indicates that CNDev has not been initialized. Make sure that ::cndevInit is successfully called.
	 */
	CNDEV_ERROR_INVALID_ARGUMENT = 5,
	/**<
	 * This indicates invalid value for parameter, such as a NULL pointer or an invalid range of values.
	 */
	CNDEV_ERROR_INVALID_DEVICE_ID = 6,
	/**<
	 * This indicates that the device handle is invalid.
	 */
	CNDEV_ERROR_UNKNOWN = 7,
	/**<
	 * This indicates an unknown internal error. Check the health status of the device or the driver.
	 */
	CNDEV_ERROR_MALLOC = 8,
	/**<
	 * This indicates the memory failed to be allocated. Make sure there is sufficient memory on the host.
	 */
	CNDEV_ERROR_INSUFFICIENT_SPACE = 9,
	/**<
	 * This indicates that CNDev does not have enough space or resources.
	 * Common cases are insufficient host memory for application, insufficient buffer passed in,
	 * and insufficient resources for device creation in MIM mode.
	 */
	CNDEV_ERROR_NOT_SUPPORTED = 10,
	/**<
	 * This indicates that the current operation is not supported.
	 * The operation may not be supported by the current platform or the driver.
	 * And this operation may not be performed at the current device status.
	 * For example, when you perform an operation on physical machines in MIM mode.
	 * You need to check the API constraints, or check the driver version or the platform.
	 * If the requirements are not met, update the environment.
	 */
	CNDEV_ERROR_INVALID_LINK = 11,
	/**<
	 * This indicates that the link port is invalid. Check the link parameter that was passed in to the API.
	 */
	CNDEV_ERROR_NO_DEVICES = 12,
	/**<
	 * This indicates that no usable MLU device is detected because the driver failed to be loaded or the
	 * device node failed to be mapped. Reload the driver or make sure the device node is successfully mapped.
	 * This error is usually returned by ::cndevInit. In this case, make sure the driver is successfully installed.
	 */
	CNDEV_ERROR_NO_PERMISSION = 13,
	/**<
	 * This indicates that the current user does not have permissions to perform this operation.
	 * This error usually occurs when configuration APIs check the privileged permissions.
	 * Check whether the operation requires the sudo or root account, and check the constraints of the API.
	 */
	CNDEV_ERROR_NOT_FOUND = 14,
	/**<
	 * This indicates that the resources or data to be requested cannot be found.
	 * For example, this error occurs when a device handle is not found, which is being obtained through UUID.
	 * Check the validity of parameters that are passed in and check the drive health status.
	 */
	CNDEV_ERROR_IN_USE = 15,
	/**<
	 * This indicates that the operation cannot be performed because the MLU device is in use.
	 * This error occurs only when you call APIs to perform setting,
	 * creating, or destroying operations, and it will not appear when you perform information-obtaining operation.
	 */
	CNDEV_ERROR_DUPLICATE = 16,
	/**<
	 * This indicates that the instance name is duplicated.
	 * This error usually occurs when you call configuration APIs.
	 * For example, this error is returned when the instance to create based on profile ID already exists.
	 * It also occurs when the register count passed to ::cndevRegisteEvents exceeds the upper limit.
	 */
	CNDEV_ERROR_TIMEOUT = 17,
	/**<
	 * This indicates that a timeout occurs.
	 * This error occurs when the operation timed out you call ::cndevEventWait or ::cndevResetDevice.
	 * For ::cndevEventWait, it means that no event is reported during the specified timeout.
	 * For ::cndevResetDevice, it usually means that the device driver does not run properly.
	 * In this case, check the device status.
	 */
	CNDEV_ERROR_IN_PROBLEM = 18,
	/**<
	 * This indicates that MLU device is in problem.
	 */
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
	CNDEV_DEVICE_TYPE_MLU585 = 27,               /**< Reserved.*/
	CNDEV_DEVICE_TYPE_1V_2201 = 29,              /**< The device is 1V_2201.*/
	CNDEV_DEVICE_TYPE_MLU580 = 30,               /**< Reserved.*/
	CNDEV_DEVICE_TYPE_MLU570 = 31,               /**< Reserved.*/
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
	MLU585 = CNDEV_DEVICE_TYPE_MLU585,           /**< Reserved.*/
	MLU580 = CNDEV_DEVICE_TYPE_MLU580,           /**< Reserved.*/
	MLU570 = CNDEV_DEVICE_TYPE_MLU570,           /**< Reserved.*/
} cndevNameEnum_t;

/** Topology relationship types.*/
typedef enum {
	SELF = 0,        /**< Same device.*/
	INTERNAL = 1,    /**< Devices that are on the same board.*/
	SINGLE = 2,      /**< Devices that only need to traverse a single PCIe switch.*/
	MULTIPLE = 3,    /**< Devices that need not traverse a host bridge.*/
	HOST_BRIDGE = 4, /**< Devices that are connected to the same host bridge.*/
	CPU = 5,         /**< Devices that are connected to the same CPU but possibly multiple host bridges.*/
	SYSTEM = 6,      /**< Devices that are in the system.*/
} cndevTopologyRelationshipEnum_t;

/** Display formats for MLU-Link speed.*/
typedef enum {
	SPEED_FMT_NRZ = 0,    /**< Transmission speed show by NRZ format.*/
	SPEED_FMT_PM4 = 1,    /**< Transmission speed show by PM4 format.*/
} cndevMLULinkSpeedEnum_t;

/**
 * @brief Count types for MLU-Link.

 * It is deprecated and will be removed in future release.
 */
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
	CNDEV_MLUINSTANCE_PROFILE_2_SLICE_MLU_1_SLICE_MEM,  /**< Slices into 2 MLU clusters and 1 memory module.*/
	CNDEV_MLUINSTANCE_PROFILE_3_SLICE,                  /**< Slices into 3 MLU clusters.*/
	CNDEV_MLUINSTANCE_PROFILE_4_SLICE,                  /**< Slices into 4 MLU clusters.*/
	CNDEV_MLUINSTANCE_PROFILE_5_SLICE,                  /**< Slices into 5 MLU clusters.*/
	CNDEV_MLUINSTANCE_PROFILE_6_SLICE,                  /**< Slices into 6 MLU clusters.*/
	CNDEV_MLUINSTANCE_PROFILE_1_SLICE_MLU_2_SLICE_MEM,  /**< Slices into 1 MLU cluster and 2 memory modules.*/
	CNDEV_MLUINSTANCE_PROFILE_COUNT,                    /**< The total number of profile types.*/
} cndevMLUInstanceProfileEnum_t;

/** Configuration types.*/
typedef enum {
	CNDEV_DEVICE_CONFIGS_WATCHDOG_TIMER = 0,	/**< Configure watchdog timer in seconds. It must be in the range [0, 1023].*/
	CNDEV_DEVICE_CONFIGS_FAST_ALLOC = 1,		/**< Configure fast allocation function. Set 0 to disable and 1 to enable.*/
	CNDEV_DEVICE_CONFIGS_COMPRESS_MEM = 2,		/**< Configure compress function. Set 0 to disable and 1 to enable.*/
	CNDEV_DEVICE_CONFIGS_LINEAR_MEM = 3,		/**< Configure linear memory. Set 0 to disable and 2 to enable.*/
	CNDEV_DEVICE_CONFIGS_IPU_LLC_DATA_ALLOCATED = 4,	/**< Configure control data cache mode in LLC for IPU.
								  * Set 0 to operate without allocating memory and 1 to allocate.
								  */
	CNDEV_DEVICE_CONFIGS_STOP_CODEC_XPU = 5,	/**< Configure codec server shutdown.
							  * Value 0 will shut down the codec server of current device.
							  */
	CNDEV_DEVICE_CONFIGS_PINNED_MEM_NUMA_MODE = 6,	/**< Configure host memory allocation with pinned NUMA node.
							  * Set 0 to disable and 1 to enable.
							  */
} cndevDeviceConfigsTypeEnum_t;

#ifdef WIN32
#define basename(file) "UNSUPPORTED"
#endif

#ifndef cndevCheckErrors
#define __cndevCheckErrors(err, file, line)                                                                                    \
	do {                                                                                                                         \
		cndevRet_t _err = (err);                                                                                                   \
		if (_err != CNDEV_SUCCESS) {                                                                                               \
			fprintf(stderr, "cndevCheckErrors(%d): %s, from file <%s>, line %i.\n",                                                  \
			_err, cndevGetErrorString(_err), basename((char *)file), line);                                                           \
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
#define CNDEV_IP_ADDRESS_LEN                   40
#define CNDEV_MLU_REMOTE_PORT_NAME_LEN         256
#define CNDEV_MAC_ADDRESS_LEN                  6
#define CNDEV_HOST_SN_LEN                      128
#define CNDEV_IFACE_NAME_LEN                   16

/** Card information.*/
typedef struct {
	__int32_t version;     /**< IN: API version.*/
	__uint32_t number;     /**< OUT: The count of devices.*/
} cndevCardInfo_t;

/** UUID information.*/
typedef struct {
	__int32_t version;                /**< IN: API version.*/
	__uint8_t uuid[CNDEV_UUID_SIZE];  /**< OUT: The information of the device UUID.*/
	__uint64_t ncsUUID64;             /**< OUT: The information of the ncs device UUID.*/
} cndevUUID_t;

/** Memory information V2.*/
typedef struct {
	__int64_t physicalMemoryTotal;    /**< OUT: The physical total memory size of MLU, unit: MB.*/
	__int64_t physicalMemoryUsed;     /**< OUT: The physical used memory size of MLU, unit: MB.*/
	__int64_t reservedMemory;         /**< OUT: The reserved memory size of MLU, unit: MB.*/
	__int64_t virtualMemoryTotal;     /**< OUT: The virtual total memory size of MLU, unit: MB.*/
	__int64_t virtualMemoryUsed;      /**< OUT: The virtual used memory size of MLU, unit: MB.*/
	__uint64_t globalMemory;          /**< OUT: The SMT memory size, unit: MB.*/
	__uint64_t reserved[16];          /**< Reserved.*/
} cndevMemoryInfoV2_t;

/** Version information.*/
typedef struct {
	__int32_t version;             /**< IN: API version.*/
	__uint32_t mcuMajorVersion;    /**< OUT: The major version number of the MCU.*/
	__uint32_t mcuMinorVersion;    /**< OUT: The minor version number of the MCU.*/
	__uint32_t mcuBuildVersion;    /**< OUT: The patch version number of the MCU.*/
	__uint32_t driverMajorVersion; /**< OUT: The major version number of MLU Driver.*/
	__uint32_t driverMinorVersion; /**< OUT: The minor version number of MLU Driver.*/
	__uint32_t driverBuildVersion; /**< OUT: The build version number of MLU Driver.*/
} cndevVersionInfo_t;

/** ECC information.*/
typedef struct {
	__int32_t version;                /**< IN: API version.*/
	__uint64_t oneBitError;           /**< OUT: The count of single single-bit errors.*/
	__uint64_t multipleOneError;      /**< OUT: The count of multiple single-bit errors.*/
	__uint64_t multipleError;         /**< OUT: The count of single multiple-bits errors.*/
	__uint64_t multipleMultipleError; /**< OUT: The count of multiple multiple-bits errors.*/
	__uint64_t correctedError;        /**< OUT: The count of corrected errors ( ``oneBitError`` + ``multipleOneError``).*/
	__uint64_t uncorrectedError;      /**< OUT: The count of uncorrected errors ( ``multipleError`` + ``multipleMultipleError``).*/
	__uint64_t totalError;            /**< OUT: Total ECC errors ( ``correctedError`` + ``uncorrectedError``).*/
	__uint64_t addressForbiddenError; /**< OUT: The count of access illegal addresses.*/
} cndevECCInfo_t;

/** ECC mode information.*/
typedef struct {
	__int32_t version;              /**< IN: API version.*/
	cndevEnableStatusEnum_t mode;   /**< IN/OUT: The status of the ECC mode.*/
} cndevEccMode_t;

/** Power information.*/
typedef struct {
	__int32_t usage;              /**< OUT: Current power dissipation of MLU device, unit: W.*/
	__int32_t cap;                /**< OUT: Current set power cap of MLU device, unit: W.*/
	__int32_t machine;            /**< OUT: Current power dissipation of OAM machine, unit: W.*/
	__int32_t tdp;                /**< OUT: Thermal design power of MLU device, unit: W.*/
	__int32_t maxPower;           /**< OUT: Maximum power dissipation of MLU device for customer evaluation of power supply, unit: W.*/
	__int32_t reserved[16];       /**< Reserved.*/
} cndevDevicePowerInfo_t;

/** Temperature information.*/
typedef struct {
	__int32_t version;			/**< IN: API version.*/
	__int32_t board;			/**< OUT: Board temperature, unit: ℃. Collects statistics on multiple temperature information
						  * on the board and reports the maximum value.
						  */
	__int32_t cluster[20];			/**< OUT: Cluster temperature, unit: ℃. To count the temperature of each IPU Core PVT sensor,
						  * take the maximum value and report it.
						  * In the case of Full MLU, the effective length is 1. Only in the case of
						  * Multi-Instance MLU virtualization, multiple temperature information will be returned.
						  */
	__int32_t memoryDie[8];			/**< reserved.*/
	__int32_t chip;				/**< OUT: Chip top temperature, unit: ℃.
						  * To collect statistics on multiple temperature information of Chip Top,
						  * the maximum value is reported.
						  */
	__int32_t airInlet;			/**< OUT: The air inlet temperature of the whole machine transmitted by BMC, unit: ℃.*/
	__int32_t airOutlet;			/**< OUT: The air outlet temperature of the whole machine transmitted by BMC, unit: ℃.*/
	__int32_t memory;			/**< OUT: The memory temperature, unit: ℃. To count the temperature of each DRAM Sys,
						  * the maximum value is reported.
						  */
	__int32_t videoInput;			/**< OUT: The temperature of video input, unit: ℃. Only edge products are valid.*/
	__int32_t cpu;				/**< OUT: The temperature of CPU, unit: ℃. Only edge products are valid.*/
	__int32_t isp;				/**< OUT: The temperature of ISP (Image Signal Processor), unit: ℃. Only edge products are valid.*/
} cndevTemperatureInfo_t;

/** Fan speed information.*/
typedef struct {
	__int32_t version;		/**< IN: API version.*/
	__int32_t fanSpeed;		/**< Reserved.*/
	__int32_t chassisFanCount;	/**< OUT: The count of chassis fans, only valid on OAM machine.*/
	__int32_t chassisFan[28];	/**< OUT: The speed information of each chassis fan, unit: rpm, only valid on OAM machine.
					  * chassisFan[0]-chassisFan[19] are 8086 fan, and chassisFan[20]-chassisFan[27] are 4028 fan.
					  */
} cndevFanSpeedInfo_t;

/** MLU utilization information.*/
typedef struct {
	__int32_t version;                /**< IN: API version.*/
	__int32_t averageCoreUtilization; /**< OUT: The average utilization of MLU core.*/
	__int32_t coreUtilization[80];    /**< OUT: The utilization of each MLU core.
					    * The valid array index range is [0, MLU core - 1].
					    */
} cndevUtilizationInfo_t;

/** MLU frequency information.*/
typedef struct {
	__int32_t version;		/**< IN: API version.*/
	__int32_t boardFreq;		/**< OUT: The current operating IPU frequency, unit: MHz.*/
	__int32_t ddrFreq;		/**< OUT: MLU DDR transfer rate, unit: MBps (operating max MLU DDR frequency, unit: MHz).*/
	__uint8_t overtempDfsFlag;	/**< OUT: Over-temperature reducing frequency flag, only valid on MLU220.*/
	__uint8_t fastDfsFlag;		/**< OUT: Fast reducing frequency flag, only valid on MLU220.*/
	__uint16_t mluClusterFreqCount;	/**< OUT: The count of MLU clusters.*/
	__uint16_t mluClusterFreq[CNDEV_MLU_CLUSTER_FREQUENCY_COUNT];	/**< OUT: The frequency of each MLU cluster.
									  * The valid array index range is [0, ``mluClusterFreqCount`` - 1],
									  * unit: MHz.
									  */
	__int32_t boardDefaultFreq;	/**< OUT: IPU default frequency, unit: MHz.*/
	__int32_t boardFreqArange[2];	/**< OUT: The range of adjustable operating frequency,
					  * boardFreqArange[0] means the minimum operating frequency,
					  * boardFreqArange[1] means the maximum operating frequency unit: MHz.
					  */
} cndevFrequencyInfo_t;

/** Process information.*/
typedef struct {
	__int32_t version;             /**< IN: API version.*/
	__uint32_t pid;                /**< OUT: Process ID.*/
	__uint64_t physicalMemoryUsed; /**< OUT: MLU physical memory used by the process, unit: KiB.*/
	__uint64_t virtualMemoryUsed;  /**< OUT: MLU virtual memory used by the process, unit: KiB.*/
} cndevProcessInfo_t;

/** Library version information.*/
typedef struct {
	__int32_t version;           /**< IN: API version.*/
	__uint32_t libMajorVersion;  /**< OUT: Library major version.*/
	__uint32_t libMinorVersion;  /**< OUT: Library minor version.*/
	__uint32_t libBuildVersion;  /**< OUT: Library build version.*/
} cndevLibVersionInfo_t;

/** Card core count.*/
typedef struct {
	__int32_t version; /**< IN: API version.*/
	__int32_t count;   /**< OUT: Core count of devices.*/
} cndevCardCoreCount_t;

/** Card cluster count.*/
typedef struct {
	__int32_t version; /**< IN: API version.*/
	__int32_t count;   /**< OUT: Cluster count of devices.*/
} cndevCardClusterCount_t;

/** Card name.*/
typedef struct {
	__int32_t version;  /**< IN: API version.*/
	cndevNameEnum_t id; /**< OUT: Card name.*/
} cndevCardName_t;

/** Serial number and related information.*/
typedef struct {
	__int32_t version;          /**< IN: API version.*/
	__uint64_t sn;              /**< OUT: Card's serial number in hexadecimal format.*/
	__uint64_t motherBoardSn;   /**< OUT: Motherboard's serial number in hexadecimal format.*/
} cndevCardSN_t;

/** Part number information of card.*/
typedef struct {
	__int32_t version;          /**< IN: API version.*/
	char partNumber[32];        /**< OUT: Part number of card.*/
} cndevCardPartNumber_t;

/** Device form.*/
typedef struct {
	__int32_t version;          /**< IN: API version.*/
	__uint64_t device_form;     /**< OUT: The form of device.*/
} cndevCardForm_t;

/** Device information of PCIe.*/
typedef struct {
	__uint32_t subsystemId;           /**< OUT: The sub-system ID of PCIe.*/
	__uint32_t deviceId;              /**< OUT: The device ID of PCIe.*/
	__uint16_t vendor;                /**< OUT: The vendor ID of PCIe.*/
	__uint16_t subsystemVendor;       /**< OUT: The sub-system Vendor ID of PCIe.*/
	__uint32_t domain;                /**< OUT: The domain number of PCIe.*/
	__uint32_t bus;                   /**< OUT: The bus number of PCIe.*/
	__uint32_t device;                /**< OUT: The device number of PCIe.*/
	__uint32_t function;              /**< OUT: The function number of PCIe.*/
	__uint16_t moduleId;              /**< OUT: The module ID on an OAM machine. This information is valid only on OAM machine.*/
	__uint16_t slotId;                /**< OUT: The physical slot ID obtained from DMI interface. 0xFFFF is an invalid slot ID.*/
	__uint32_t reserved[8];           /**< Reserved.*/
} cndevPCIeInfoV2_t;

/** PCie throughput information.*/
typedef struct {
	__int64_t pcieRead;         /**< OUT: The throughput of reading data in the past 20ms, unit: Byte.*/
	__int64_t pcieWrite;        /**< OUT: The throughput of writing data in the past 20ms, unit: Byte.*/
	__int64_t pcieReadTotal;    /**< OUT: The total throughput of reading data since load driver, unit: Byte.*/
	__int64_t pcieWriteTotal;   /**< OUT: The total throughput of writing data since load driver, unit: Byte.*/
	__uint64_t timestamp;       /**< OUT: The timestamp of throughput since 1970 (in microseconds).*/
	__int64_t reserved[8];      /**< Reserved.*/
} cndevPCIethroughputV2_t;

/** Device affinity information.*/
typedef struct {
	__int32_t version;	/**< IN: API version.*/
	__uint32_t cpuCount;	/**< OUT: The CPU count.*/
	__uint32_t cpuAffinityBitMap[1024];	/**< OUT: If there are 80 CPUs in the system, bitmap's format is:
						  * bitmap[0]:31-16:not used, 15-0:cpu79-cpu64
						  * bitmap[1]:31-0:cpu63-cpu32
						  * bitmap[2]:31-0:cpu31-cpu0.
						  */
} cndevAffinity_t;

/** Topology relationship information.*/
typedef struct {
	__int32_t version;                        /**< IN: API version.*/
	cndevTopologyRelationshipEnum_t relation; /**< OUT: The topology relation.*/
} cndevTopologyRelationship_t;

/** Device current PCI information.*/
typedef struct {
	__int32_t version;      /**< IN: API version.*/
	__int32_t currentSpeed; /**< OUT: PCIe generation.*/
	__int32_t currentWidth; /**< OUT: PCIe link width.*/
} cndevCurrentPCIInfo_t;

/** Device Max PCI information.**/
typedef struct {
	__int32_t version;     /**< IN: API version.*/
	__int32_t maxSpeed;    /**< OUT: PCIe generation.*/
	__int32_t maxWidth;    /**< OUT: PCIe link width.*/
} cndevPCIInfo_t;

/** Topology node capability information.*/
typedef struct cndevTopologyNodeCapInfo_t {
	struct cndevTopologyNodeCapInfo_t *next;         /**< OUT: The pointer of the next node.*/
	__uint8_t id;                             /**< OUT: The capability ID of the current node.*/
	__uint16_t cap;                           /**< OUT: The capability value of the current node.*/
} cndevTopologyNodeCapInfo_t;

/** Topology node information.*/
typedef struct cndevTopologyNode_t {
	__int32_t virtualRootNode;      /**< OUT: The value is set to 1 when it is the virtual root node, and set to 0 when it is the real node.*/
	__int32_t domain;               /**< OUT: Domain in PCIe.*/
	__int32_t bus;                  /**< OUT: Bus number in PCIe.*/
	__int32_t device;               /**< OUT: Device in PCIe.*/
	__int32_t function;             /**< OUT: Function in PCIe.*/
	__uint32_t subsystemId;         /**< OUT: Subsystem ID.*/
	__uint32_t deviceId;            /**< OUT: Device ID of PCIe.*/
	__uint32_t vendor;              /**< OUT: Vendor ID of PCIe.*/
	__uint32_t subsystemVendor;     /**< OUT: Subsystem vendor ID of PCIe.*/
	char const *deviceName;         /**< OUT: Device name.*/
	__uint32_t classVal;            /**< OUT: Value of the class register of PCIe.*/
	char const *className;          /**< OUT: Class name.*/
	struct cndevTopologyNodeCapInfo_t *firstCap; /**< OUT: First capability.*/
	struct cndevTopologyNode_t *parent;          /**< OUT: Parent node of this node.*/
	struct cndevTopologyNode_t *left;            /**< OUT: Left node of this node.*/
	struct cndevTopologyNode_t *right;           /**< OUT: Right node of this node.*/
	struct cndevTopologyNode_t *child;           /**< OUT: Child node of this node.*/
	__uint32_t linkSpeed;     /**< OUT: Link Speed of the device.*/
	__int32_t isBridge;       /**< OUT: The value is set to true when the component is a bridge, and set to false when it is not a bridge.*/
	__int32_t isCardbus;      /**< OUT: The value is set to true when the component is a CardBus, and set to false when it is not a CardBus.*/
	__uint8_t primaryBus;     /**< OUT: Primary Bus.*/
	__uint8_t secondaryBus;   /**< OUT: Secondary Bus.*/
	__uint8_t subordinateBus; /**< OUT: Subordinate Bus.*/
	__int32_t acsCtrl;        /**< OUT: ACS control information.*/
} cndevTopologyNode_t;

/** Topology node capability information.*/
typedef struct {
	__int32_t version;      /**< IN: API version.*/
	__uint8_t id;           /**< IN: Capability ID.*/
	__uint16_t cap;         /**< OUT: Capability value.*/
} cndevCapabilityInfo_t;

/** Health status of the device.*/
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
	__int32_t version;                           /**< IN: API version.*/
	__int32_t health;                                  /**< OUT: The health flag.*/
	cndevHealthStateEnum_t deviceState;          /**< OUT: The health status of the device.*/
	cndevDriverHealthStateEnum_t driverState;    /**< OUT: The health status of MLU Driver.*/
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
	CNDEV_FR_ADDRESSSWAP_FAILURE,             /**< Address swapping failed.*/
	CNDEV_FR_PENDING_ADDRESSSWAP,             /**< Address swapping is pending.*/
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
	char msg[CNDEV_ERR_MSG_LENGTH];      /**< OUT: Messages for the error codes.*/
	__uint32_t device_id;                /**< OUT: Device ID of the incidents.*/
	__uint32_t bdf;                      /**< OUT: Device BDF of the incidents.*/
	cndevHealthError_t code;             /**< OUT: Error codes of the incidents.*/
	__uint32_t category;                 /**< Reserved.*/
	__uint32_t severity;                 /**< Reserved.*/
} cndevDiagErrorDetail_t;

/** Health diagnosis incident information.*/
typedef struct {
	cndevHealthSystem_t system;          /**< OUT: System to which this information belongs.*/
	cndevHealthResult_t health;          /**< OUT: Health diagnosis of this incident.*/
	cndevDiagErrorDetail_t error;        /**< OUT: Information about this error and its error code.*/
} cndevIncidentInfo_t;

#define CNDEV_HEALTH_SYSTEM_MAX_INCIDENTS 64

/** Device and MLU Driver health information, which includes the health incidents of each system.*/
typedef struct {
	__int32_t health;                                                  /**< OUT: The health flag.*/
	cndevHealthStateEnum_t deviceState;                                /**< OUT: The health status of the device.*/
	cndevDriverHealthStateEnum_t driverState;                          /**< OUT: The health status of MLU Driver.*/
	cndevHealthResult_t overallHealth;                                 /**< OUT: The overall health status.*/
	__uint32_t incident_count;                                         /**< OUT: The number of health incidents reported.*/
	cndevIncidentInfo_t incidents[CNDEV_HEALTH_SYSTEM_MAX_INCIDENTS];  /**< OUT: Report of the errors detected.*/
	__uint32_t reserved[8];                                            /**< Reserved.*/
} cndevCardHealthStateV2_t;

/** Heartbeat information.*/
typedef struct {
	__int32_t version;          /**< IN: API version.*/
	__uint32_t heartbeatCount;  /**< OUT: The count of heartbeats.*/
} cndevCardHeartbeatCount_t;

/** Device Link speed.*/
typedef struct {
	__int32_t version;     /**< IN: API version.*/
	__int32_t linkSpeed;   /**< OUT: The speed that can be 1-2.5GT/s, 2-5GT/s, 3-8GT/s, 4-16GT/s or 5-32GT/s.*/
} cndevLinkSpeed_t;

/** Device VPU utilization.*/
typedef struct {
	__int32_t version;                /**< IN: API version.*/
	__int32_t vpuCount;               /**< OUT: The count of VPUs.*/
	__int32_t vpuCodecUtilization[20];/**< OUT: The utilization of each VPU. The valid array index range is [0, ``vpuCount`` - 1].*/
	__int32_t encoderCount;           /**< OUT: The count of video encoders.*/
	__int32_t encoderUtilization[20]; /**< OUT: The utilization of each encoder. The valid array index range is [0, ``encoderCount`` - 1].*/
	__int32_t decoderCount;           /**< OUT: The count of video decoders.*/
	__int32_t decoderUtilization[20]; /**< OUT: The utilization of each decoder. The valid array index range is [0, ``decoderCount`` - 1].*/
} cndevVideoCodecUtilization_t;

/** Device JPU utilization.*/
typedef struct {
	__int32_t version;                /**< IN: API version.*/
	__int32_t jpuCount;               /**< OUT: The count of JPUs.*/
	__int32_t jpuCodecUtilization[20];/**< OUT: The utilization of each JPU. The valid array index range is [0, ``jpuCount`` - 1].*/
} cndevImageCodecUtilization_t;

/** Device Fast memory allocation.*/
typedef struct {
	__int32_t version;          /**< IN: API version.*/
	__int32_t fastMemoryTotal;  /**< OUT: The total size of fast memory in KByte.*/
	__int32_t fastMemoryFree;   /**< OUT: The size of free fast memory available for allocation in KByte.*/
} cndevFastAlloc_t;

/** NUMA node ID.*/
typedef struct {
	__int32_t version;   /**< IN: API version.*/
	__int32_t nodeId;    /**< OUT: NUMA node ID.*/
} cndevNUMANodeId_t;

/** Device Scaler utilization.*/
typedef struct {
	__int32_t version;                /**< IN: API version.*/
	__int32_t scalerCount;            /**< OUT: The count of scalers.*/
	__int32_t scalerUtilization[20];  /**< OUT: The utilization of each scaler. The valid array index range is [0, ``scalerCount`` - 1].*/
} cndevScalerUtilization_t;

/** Codec Turbo information.*/
typedef struct {
	__int32_t version;        /**< IN: API version.*/
	__int32_t codecTurbo;     /**< OUT: Codec Turbo status. \p 0 represents \p DISABLE, and \p 1 represents \p ENABLE.*/
} cndevCodecTurbo_t;

/** MLU-Link version.*/
typedef struct {
	__int32_t version;          /**< IN: API version.*/
	__uint32_t majorVersion;    /**< OUT: The major version number.*/
	__uint32_t minorVersion;    /**< OUT: The minor version number.*/
	__uint32_t buildVersion;    /**< OUT: The patch version number.*/
} cndevMLULinkVersion_t;


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
	cndevMLULinkMACStateEnum_t macState;		/**< OUT: The MAC status of the MLU-Link.*/
	cndevMLULinkSerdesStateEnum_t serdesState;	/**< OUT: The SerDes status of the MLU-Link.*/
	cndevMLULinkPresenceStateEnum_t presenceState;	/**< OUT: The Presence status of the MLU-Link.*/
	__uint32_t serdesProgress;			/**< OUT: The status of establishing an MLU-Link.
							  * It is valid when \p serdesState is CNDEV_MLULINK_SERDES_STATE_ONGOING.
							  * The valid value is in range of [0, 1000], where 1000 indicates the link is established.
							  */
	__uint32_t reserved[7];				/**< Reserved.*/
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
	__uint8_t qdd_idx;			/**< OUT: The scale out port index.*/
	__uint8_t split_idx;			/**< OUT: The splitting index of the optical module.*/
	__uint8_t present;			/**< OUT: Installation status of the optical module.*/
	char vendor_name[16];			/**< OUT: Vendor name.*/
	char vendor_pn[16];			/**< OUT: Vendor device ID.*/
	char vendor_sn[16];			/**< OUT: Vendor serial number.*/
	__uint8_t vendor_oui[3];		/**< OUT: Unique identifier of the vendor organization.*/
	__uint8_t identifier;			/**< OUT: Identity information of the optical module.*/
	float wave_length;			/**< OUT: Optical module wavelength, unit: nm.*/
	char manu_dc[8];			/**< OUT: Manufacturing date code.*/
	float temp;				/**< OUT: Optical module temperature, degrees Celsius.*/
	cndevOpticalStatusEnum_t temp_state;	/**< OUT: The temperature status of optical module.*/
	cndevOpticalThres_t temp_thresh;	/**< OUT: Temperature monitor thresholds.*/
	float volt;				/**< OUT: Power voltage of the optical module, unit: V.*/
	cndevOpticalStatusEnum_t volt_state;	/**< OUT: The voltage status of optical module.*/
	cndevOpticalThres_t volt_thresh;	/**< OUT: Supply 3.3-volt monitor thresholds.*/
	__int32_t valid_lane_num;		/**< OUT: The valid number of lane information for
						  * ``txpwr``, ``txpwr_state``, ``txbias``, ``txbias_state``, ``rxpwr`` and ``rxpwr_state``.
						  */
	float txpwr[8];				/**< OUT: TX[1:8] output optical power, unit: dBm.*/
	cndevOpticalStatusEnum_t txpwr_state[8];/**< OUT: TX[1:8] optical power status.*/
	cndevOpticalThres_t txpwr_thresh;	/**< OUT: TX optical power monitor thresholds.*/
	float txbias[8];			/**< OUT: TX[1:8] bias current, unit: mA.*/
	cndevOpticalStatusEnum_t txbias_state[8];/**< OUT: TX[1:8] bias current status.*/
	cndevOpticalThres_t txbias_thresh;	/**< OUT: TX bias current monitor thresholds.*/
	float rxpwr[8];				/**< OUT: RX[1:8] input optical power, unit: dBm.*/
	cndevOpticalStatusEnum_t rxpwr_state[8];/**< OUT: RX[1:8] optical power status.*/
	cndevOpticalThres_t rxpwr_thresh;	/**< OUT: RX optical power thresholds.*/
	__uint8_t txlos_flag;			/**< OUT: TX[1:8] optical power loss flag.*/
	__uint8_t rxlos_flag;			/**< OUT: RX[1:8] optical power loss flag.*/
	__uint64_t reserved[8];		/**< Reserved.*/
} cndevOpticalInfo_t;

/** MLU-Link speed.*/
typedef struct {
	__int32_t version;                    /**< IN: API version.*/
	float speedValue;                     /**< OUT: The max speed of MLU-Link port, unit: GB/s.*/
	cndevMLULinkSpeedEnum_t speedFormat;  /**< OUT: The format of speed.*/
} cndevMLULinkSpeed_t;

/** MLU-Link capability, only valid on MLU370.*/
typedef struct {
	__int32_t version;            /**< IN: API version.*/
	__uint32_t p2pTransfer;       /**< OUT: The capability of p2p Transfer.*/
	__uint32_t interlakenSerdes;  /**< OUT: The capability of interlaken SerDes.*/
} cndevMLULinkCapability_t;

/**
 * @brief MLU-Link statistical count.

 * It is deprecated and will be removed in future release.
 */
typedef struct {
	__int32_t version;              /**< IN: API version.*/
	__uint64_t cntrReadByte;        /**< OUT: The count of read bytes.*/
	__uint64_t cntrReadPackage;     /**< OUT: The count of read packages.*/
	__uint64_t cntrWriteByte;       /**< OUT: The count of write bytes.*/
	__uint64_t cntrWritePackage;    /**< OUT: The count of write packages.*/
	__uint64_t errReplay;           /**< OUT: The count of replay packages.*/
	__uint64_t errFatal;            /**< OUT: The count of failures.*/
	__uint64_t errEccDouble;        /**< OUT: The count of ECC Double errors.*/
	__uint64_t errCRC24;            /**< OUT: The count of CRC24 errors.*/
	__uint64_t errCRC32;            /**< OUT: The count of CRC32 errors.*/
	__uint64_t errCorrected;        /**< OUT: The count of corrected errors.*/
	__uint64_t errUncorrected;      /**< OUT: The count of uncorrected errors.*/
	__uint64_t errReadPackage;      /**< OUT: The count of failed attempts to read packages.*/
	__uint64_t errWritePackage;     /**< OUT: The count of failed attempts to write packages.*/
	__uint64_t errIllegalAccess;    /**< OUT: The count of illegal access.*/
	__uint64_t cntrCnpPackage;      /**< OUT: The count of CNP packages.*/
	__uint64_t cntrPfcPackage;      /**< OUT: The count of PFC packages.*/
	__uint64_t statQPInUsing;       /**< OUT: The count of QPs in use.*/
	__uint64_t statTaskAlive;       /**< OUT: The count of active tasks.*/
} cndevMLULinkCounter_t;

/** MLU-Link statistics information.*/
typedef struct {
	__uint64_t rxPfcOkPkt;            /**< OUT: The total count of received OK PFC packets.*/
	__uint64_t rxPfc0Pkt;             /**< OUT: The count of received PFC packets with priority 0.*/
	__uint64_t rxPfc1Pkt;             /**< OUT: The count of received PFC packets with priority 1.*/
	__uint64_t rxPfc2Pkt;             /**< OUT: The count of received PFC packets with priority 2.*/
	__uint64_t rxPfc3Pkt;             /**< OUT: The count of received PFC packets with priority 3.*/
	__uint64_t rxPfc4Pkt;             /**< OUT: The count of received PFC packets with priority 4.*/
	__uint64_t rxPfc5Pkt;             /**< OUT: The count of received PFC packets with priority 5.*/
	__uint64_t rxPfc6Pkt;             /**< OUT: The count of received PFC packets with priority 6.*/
	__uint64_t rxPfc7Pkt;             /**< OUT: The count of received PFC packets with priority 7.*/
	__uint64_t txPfcOkPkt;            /**< OUT: The total count of sent OK PFC packets.*/
	__uint64_t txPfc0Pkt;             /**< OUT: The count of sent PFC packets with priority 0.*/
	__uint64_t txPfc1Pkt;             /**< OUT: The count of sent PFC packets with priority 1.*/
	__uint64_t txPfc2Pkt;             /**< OUT: The count of sent PFC packets with priority 2.*/
	__uint64_t txPfc3Pkt;             /**< OUT: The count of sent PFC packets with priority 3.*/
	__uint64_t txPfc4Pkt;             /**< OUT: The count of sent PFC packets with priority 4.*/
	__uint64_t txPfc5Pkt;             /**< OUT: The count of sent PFC packets with priority 5.*/
	__uint64_t txPfc6Pkt;             /**< OUT: The count of sent PFC packets with priority 6.*/
	__uint64_t txPfc7Pkt;             /**< OUT: The count of sent PFC packets with priority 7.*/
	__uint64_t rxRocePkt;             /**< OUT: The count of packets received by RoCE.*/
	__uint64_t txRocePkt;             /**< OUT: The count of packets sent by RoCE.*/
	__uint64_t roceVerificationErrPkt;/**< OUT: The count of error packets verified by RoCE.*/
	__uint64_t reserved[64];          /**< Reserved.*/
} cndevMLULinkCounterExt_t;

/** MLU-Link basic counter information.*/
typedef struct {
	__uint64_t rxByte;           /**< OUT: The total number of bytes of all packets received with no error.*/
	__uint64_t rxPkt;            /**< OUT: The count of all packets received with no error.*/
	__uint64_t txByte;           /**< OUT: The total number of bytes of all packets transmitted with no error.*/
	__uint64_t txPkt;            /**< OUT: The count of all packets sent with no error.*/
	__uint64_t rxRocePkt;        /**< OUT: The count of all packets received by RoCE with no error.*/
	__uint64_t txRocePkt;        /**< OUT: The count of all packets transmitted by RoCE with no error.*/
	__uint64_t timestamp;        /**< OUT: The timestamp of getting basic counters since 1970 (in microseconds).*/
	__uint64_t reserved[25];     /**< Reserved.*/
} cndevMLULinkBasicCounter_t;

/** MLU-Link congestion control counter information.*/
typedef struct {
	__uint64_t rxCnpPkt;      /**< OUT: The count of Congestion Notification Packets received.*/
	__uint64_t reserved[7];   /**< Reserved.*/
} cndevMLULinkCongestionCtrlCounter_t;

/** MLU-Link event counter information.*/
typedef struct {
	__uint64_t correctFecCnt;       /**< OUT: The count of corrected codewords by FEC correction algorithms.*/
	__uint64_t uncorrectFecCnt;     /**< OUT: The count of uncorrected codewords by FEC correction algorithms.*/
	__uint64_t rxBadFcsPkt;         /**< OUT: The total count of packets received with bad FCS.*/
	__uint64_t txBadFcsPkt;         /**< OUT: The total count of packets transmitted with bad FCS.*/
	__uint64_t illegalAccessCnt;    /**< OUT: The count of illegal memory accesses detected while receiving data.*/
	__uint64_t reserved[11];        /**< Reserved.*/
} cndevMLULinkErrorCounter_t;

/** MLU-Link flow control counter information.*/
typedef struct {
	__uint64_t rxPfcPkt;         /**< OUT: The total count of received PFC packets.*/
	__uint64_t rxPfc0Pkt;        /**< OUT: The count of received PFC packets with priority 0.*/
	__uint64_t rxPfc1Pkt;        /**< OUT: The count of received PFC packets with priority 1.*/
	__uint64_t rxPfc2Pkt;        /**< OUT: The count of received PFC packets with priority 2.*/
	__uint64_t rxPfc3Pkt;        /**< OUT: The count of received PFC packets with priority 3.*/
	__uint64_t rxPfc4Pkt;        /**< OUT: The count of received PFC packets with priority 4.*/
	__uint64_t rxPfc5Pkt;        /**< OUT: The count of received PFC packets with priority 5.*/
	__uint64_t rxPfc6Pkt;        /**< OUT: The count of received PFC packets with priority 6.*/
	__uint64_t rxPfc7Pkt;        /**< OUT: The count of received PFC packets with priority 7.*/
	__uint64_t txPfcPkt;         /**< OUT: The total count of sent OK PFC packets.*/
	__uint64_t txPfc0Pkt;        /**< OUT: The count of sent PFC packets with priority 0.*/
	__uint64_t txPfc1Pkt;        /**< OUT: The count of sent PFC packets with priority 1.*/
	__uint64_t txPfc2Pkt;        /**< OUT: The count of sent PFC packets with priority 2.*/
	__uint64_t txPfc3Pkt;        /**< OUT: The count of sent PFC packets with priority 3.*/
	__uint64_t txPfc4Pkt;        /**< OUT: The count of sent PFC packets with priority 4.*/
	__uint64_t txPfc5Pkt;        /**< OUT: The count of sent PFC packets with priority 5.*/
	__uint64_t txPfc6Pkt;        /**< OUT: The count of sent PFC packets with priority 6.*/
	__uint64_t txPfc7Pkt;        /**< OUT: The count of sent PFC packets with priority 7.*/
	__uint64_t reserved[14];     /**< Reserved.*/
} cndevMLULinkFlowCtrlCounter_t;

/** MLU-Link event counter information.*/
typedef struct {
	__uint64_t linkDown;        /**< OUT: The count of MLU-Link disconnections.*/
	__uint64_t replay;          /**< OUT: The total count of task replays.*/
	__uint64_t replayFail;      /**< OUT: The count of task replay failures.*/
	__uint64_t reserved[13];    /**< Reserved.*/
} cndevMLULinkEventCounter_t;

/** MLU-Link task statistics counter information.*/
typedef struct {
	__uint64_t qpInUsing;       /**< OUT: The count of QPs in use on an MLU-Link.*/
	__uint64_t taskAlive;       /**< OUT: The count of tasks on an MLU-Link.*/
	__uint64_t reserved[14];    /**< Reserved.*/
} cndevMLULinkTaskStatsCounter_t;

/**
 * @brief MLU-Link count information.

 * It is deprecated and will be removed in future release.
 */
typedef struct {
	__int32_t version;                    /**< IN: API version.*/
	cndevMLULinkCounterEnum_t setCounter; /**< IN: The count type.*/
} cndevMLULinkSetCounter_t;

/** MLU-Link remote information.*/
typedef struct {
	__int32_t version;                          /**< IN: API version.*/
	__uint64_t mcSn;                            /**< OUT: The mezzanine device SN of the device that is connected with MLU-Link.*/
	__uint64_t baSn;                            /**< OUT: The board SN of the device that is connected with MLU-Link.*/
	__uint32_t slotId;                          /**< OUT: The slot ID of the device that is connected with MLU-Link.*/
	__uint32_t portId;                          /**< OUT: The port ID of the device that is connected with MLU-Link.*/
	__uint8_t devIp[16];                        /**< OUT: The IP address of the device that is connected with MLU-Link.*/
	__uint8_t uuid[CNDEV_UUID_SIZE];            /**< OUT: The UUID of the device that is connected with MLU-Link.*/
	__uint32_t devIpVersion;                    /**< OUT: The IP version of the device that is connected with MLU-Link.*/
	__uint32_t isIpValid;                       /**< OUT: The valid flag of the IP address of the device that is connected with MLU-Link.*/
	__int32_t connectType;                      /**< OUT: The connection type of the device that is connected with MLU-Link.*/
	__uint64_t ncsUUID64;                       /**< OUT: The ncs UUID of the device that is connected with MLU-Link.*/
	__uint8_t  mac_addr[CNDEV_MAC_ADDRESS_LEN]; /**< OUT: The MAC address of the device that is connected with MLU-Link.*/
	char port_name[CNDEV_MLU_REMOTE_PORT_NAME_LEN];  /**< OUT: The port name of the device that is connected with MLU-Link.*/
} cndevMLULinkRemoteInfo_t;

/** MLU-Link management network address.*/
typedef struct {
	__uint32_t ip_version;           /**< OUT: IP address version. 4: IPv4, 6: IPv6, and 1: IPv4 and IPv6.*/
	struct in_addr ip_addr;          /**< OUT: IPv4 address, valid when ip_version is IPv4 or IPv4 and IPv6.*/
	struct in6_addr ipv6_addr;       /**< OUT: IPv6 address, valid when ip_version is IPv6 or IPv4 and IPv6.*/
} cndevMLULinkMgmtAddr_t;

/** MLU-Link host SN.*/
typedef struct {
	char sn[CNDEV_HOST_SN_LEN]; /**< OUT: Serial number.*/
} cndevMLULinkHostSN_t;

/** MLU-Link port physical index name.*/
typedef struct {
	char ppi[CNDEV_IFACE_NAME_LEN];		/**< OUT: Port physical index name by main index and spilt index, such as 0:1.
						  * It is "N/A" for non scale-out MLU-Links.
						  */
} cndevMLULinkPPI_t;

/** MLU-Link OAM port name.*/
typedef struct {
	char opn[CNDEV_IFACE_NAME_LEN]; /**< OUT: OAM port name.*/
} cndevMLULinkOPN_t;

/** MLU-Link devices SN.*/
typedef struct {
	__int32_t version;               /**< IN: API version.*/
	__uint64_t mlulinkMcSn[8];       /**< OUT: The mezzanine device SN of the device that is connected with MLU-Link.*/
	__uint64_t mlulinkBaSn[8];       /**< OUT: The board SN of the device that is connected with MLU-Link.*/
} cndevMLULinkDevSN_t;

/** NVME information.*/
typedef struct {
	__uint8_t nvmeSn[21];             /**< OUT: The NVME SSD SN.*/
	__uint8_t nvmeModel[17];          /**< OUT: The NVME SSD model.*/
	__uint8_t nvmeFw[9];              /**< OUT: The NVME SSD firmware version.*/
	__uint8_t nvmeMfc[9];             /**< OUT: The NVME SSD manufacturer.*/
} cndevNvmeSsdInfo_t;

/** PSU information.*/
typedef struct {
	__uint8_t psuSn[17];              /**< OUT: The PSU SN.*/
	__uint8_t psuModel[17];           /**< OUT: The PSU model.*/
	__uint8_t psuFw[17];              /**< OUT: The PSU firmware version.*/
	__uint8_t psuMfc[17];             /**< OUT: The PSU manufacturer.*/
} cndevPsuInfo_t;

/** IB information.*/
typedef struct {
	__uint8_t ibSn[25];               /**< OUT: The IB SN.*/
	__uint8_t ibModel[17];            /**< OUT: The IB model.*/
	__uint8_t ibFw[5];                /**< OUT: The IB firmware version.*/
	__uint8_t ibMfc[9];               /**< OUT: The IB manufacturer.*/
} cndevIbInfo_t;

/** Chassis information.*/
typedef struct {
	__int32_t version;                     /**< IN: API version.*/
	__uint64_t chassisSn;                  /**< OUT: Chassis SN.*/
	char chassisProductDate[12];           /**< OUT: Chassis product date.*/
	char chassisPartNum[13];               /**< OUT: Chassis production model.*/
	char chassisVendorName[17];            /**< OUT: Chassis manufacturer.*/
	__uint8_t nvmeSsdNum;                  /**< OUT: The count of NVME SSDs.*/
	cndevNvmeSsdInfo_t nvmeInfo[8];        /**< OUT: The information of NVME SSD.*/
	__uint8_t ibBoardNum;                  /**< OUT: The count of IBs.*/
	cndevIbInfo_t ibInfo[8];               /**< OUT: The information of IB.*/
	__uint8_t psuNum;                      /**< OUT: The count of PSUs.*/
	cndevPsuInfo_t psuInfo[6];             /**< OUT: The information of PSU.*/
	__uint8_t bmcIPVersion;                /**< OUT: The Falcon chassis BMC IP address version.*/
	__uint8_t bmcIP[CNDEV_IP_ADDRESS_LEN]; /**< OUT: The Falcon chassis BMC IP address.*/
} cndevChassisInfo_t;

/** Chassis information V2.*/
typedef struct {
	__int32_t version;                      /**< IN: API version.*/
	__uint64_t chassisSn;                   /**< OUT: Chassis SN.*/
	char chassisProductDate[12];            /**< OUT: Chassis product date.*/
	char chassisProductName[13];            /**< OUT: Chassis product name.*/
	char chassisVendorName[17];             /**< OUT: Chassis manufacturer.*/
	__uint8_t nvmeSsdNum;                   /**< OUT: The count of NVME SSDs.*/
	cndevNvmeSsdInfo_t nvmeInfo[8];         /**< OUT: The information of NVME SSD.*/
	__uint8_t ibBoardNum;                   /**< OUT: The count of IBs.*/
	cndevIbInfo_t ibInfo[8];                /**< OUT: The information of IB.*/
	__uint8_t psuNum;                       /**< OUT: The count of PSUs.*/
	cndevPsuInfo_t psuInfo[6];              /**< OUT: The information of PSU.*/
	__uint8_t bmcIPVersion;                 /**< OUT: The Falcon chassis BMC IP address version.*/
	__uint8_t bmcIP[CNDEV_IP_ADDRESS_LEN];  /**< OUT: The Falcon chassis BMC IP address.*/
	char chassisPartNumber[32];             /**< OUT: Chassis part number.*/
	__uint64_t reserved[16];                /**< Reserved.*/
} cndevChassisInfoV2_t;

/** PCIe firmware information.*/
typedef struct {
	__int32_t version;                /**< IN: API version.*/
	__uint16_t pcieReversion;         /**< OUT: PCIe firmware reversion.*/
	__uint16_t pcieBuildID;           /**< OUT: PCIe firmware build ID.*/
	__uint16_t pcieEngineeringId;     /**< OUT: PCIe firmware engineering ID.*/
} cndevPCIeFirmwareVersion_t;

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
	__int32_t version;                               /**< IN: API version.*/
	__uint16_t chipUtilization;                      /**< OUT: The average utilization of CPU.*/
	__uint8_t coreNumber;                            /**< OUT: The count of CPU cores.*/
	__uint8_t coreUtilization[CNDEV_MAX_CORE_COUNT]; /**< OUT: The utilization of each CPU core.
							   * The valid array index range is [0, ``coreNumber`` - 1].
							   */
	__uint8_t coreStateUtilization[CNDEV_MAX_CORE_COUNT][CNDEV_CPUSTATE_MAX]; /**< OUT: The detailed state percentage for each CPU core.*/
} cndevDeviceCPUUtilizationV2_t;

/** Device CPU sampling interval.*/
typedef struct {
	__int32_t version;                /**< IN: API version.*/
	__uint32_t samplingInterval;      /**< IN/OUT: The sampling interval for CPU utilization, unit: ms.*/
} cndevDeviceCPUSamplingInterval_t;

/** Reasons of retired page.*/
typedef enum {
	CNDEV_PAGE_RETIREMENT_CAUSE_MULTIPLE_SINGLE_BIT_ECC_ERRORS = 0, /**< Page retirement due to multiple single-bit ECC error.*/
	CNDEV_PAGE_RETIREMENT_CAUSE_DOUBLE_BIT_ECC_ERROR = 1            /**< Page retirement due to double bit ECC error.*/
} cndevRetirePageCauseEnum_t;

/** Retired page information.*/
typedef struct {
	__int32_t version;                  /**< IN: API version.*/
	cndevRetirePageCauseEnum_t cause;   /**< IN: The cause is input parameter, which represents the reason of retired page.*/
	__uint32_t pageCount;               /**< OUT: The number of retired pages matching specific cause.*/
	__uint64_t pageAddress[512];        /**< OUT: The list of physical addresses for the retired pages.*/
} cndevRetiredPageInfo_t;

/** Retire page status.*/
typedef struct {
	__int32_t version;		/**< IN: API version.*/
	__uint32_t isPending;		/**< OUT: Flag indicating if the page retire operation is pending.
					  * True if a page retire process is pending. False otherwise.
					  */
	__uint32_t isFailure;		/**< OUT: Flag indicating if the page retire operation failed.
					  * True if an error occurred during the page retire process. False otherwise.
					  */
} cndevRetiredPageStatus_t;

/** X-axis labels of repair resource histogram.
 * This value varies in different structures: In @p cndevAddressSwap_t, it represents the number of
 * channels in different repair resource usage situations, and in @p cndevRemappedRowV2_t, it represents
 * the number of banks in different repair resource usage situations.
 */
typedef enum {
	CNDEV_AVAILABILITY_XLABLE_NONE,    /**< Indicates the number of banks/channels where repair resources are used up.*/
	CNDEV_AVAILABILITY_XLABLE_PARTIAL, /**< Indicates the number of banks/channels where repair resources are partially used.*/
	CNDEV_AVAILABILITY_XLABLE_MAX,     /**< Indicates the number of banks/channels where repair resources are unused.*/
	CNDEV_AVAILABILITY_XLABLE_ALL,     /**< The number of Xlable members.*/
} cndevRepairHistogramXlableEnum_t;

/** Remaped row information.*/
typedef struct {
	__int32_t version;          /**< IN: API version.*/
	__uint32_t correctRows;     /**< OUT: The count of row remapping resources used by repairing correctable memory errors.*/
	__uint32_t uncorrectRows;   /**< OUT: The count of row remapping resources used by repairing uncorrectable memory errors.*/
	__uint32_t pendingRows;     /**< Reserved.*/
	__uint32_t failedRows;      /**< Reserved.*/
} cndevRemappedRow_t;

/** The remapped row information.*/
typedef struct {
	__int32_t version;            /**< IN: API version.*/
	__uint32_t correctCounts;     /**< OUT: The count of memory error repair resources used by repairing correctable memory errors.*/
	__uint32_t uncorrectCounts;   /**< OUT: The count of memory error repair resources used by repairing uncorrectable memory errors.*/
	__uint32_t histogram[CNDEV_AVAILABILITY_XLABLE_ALL]; /**< OUT: The histogram value of memory error repair resources.*/
} cndevRemappedRowV2_t;

/** Memory error repair status.*/
typedef struct {
	__int32_t version;	/**< IN: API version.*/
	bool isRetirePending;	/**< OUT: Flag indicating if the memory error retire operation is pending.
				  * True if a retire process is pending. False otherwise.
				  */
	bool isPending;		/**< OUT: Flag indicating if the memory error repair operation is pending.
				  * True if a repair process is pending. False otherwise.
				  */
	bool isFailure;		/**< OUT: Flag indicating if the memory error repair operation failed.
				  * True if an error occurred during the repair process. False otherwise.
				  */
} cndevRepairStatus_t;

/** Address swapping information.*/
typedef cndevRemappedRowV2_t cndevAddressSwap_t;

/** The remapped row information.*/
typedef struct {
	__uint32_t correctCounts;     /**< OUT: The count of memory error repair resources used by repairing correctable memory errors.*/
	__uint32_t uncorrectCounts;   /**< OUT: The count of memory error repair resources used by repairing uncorrectable memory errors.*/
	__uint32_t histogramMax;      /**< OUT: The number of channels where repair resources are unused.*/
	__uint32_t histogramHigh;     /**< OUT: The number of channels where the remaining repair resources are more.*/
	__uint32_t histogramPartial;  /**< OUT: The number of channels where repair resources are partially used.*/
	__uint32_t histogramLow;      /**< OUT: The number of channels where the remaining repair resources are fewer.*/
	__uint32_t histogramNone;     /**< OUT: The number of channels where repair resources are used up.*/
	__uint32_t reserved[6];       /**< Reserved.*/
} cndevAddressSwapV2_t;

/** Retire page switch.*/
typedef struct {
	__int32_t version;                          /**< IN: API version.*/
	cndevEnableStatusEnum_t retirePageOption;   /**< OUT: The enable of retire page.*/
} cndevRetiredPageOperation_t;

/** VF status.*/
typedef struct {
	__int32_t version;        /**< IN: API version.*/
	__int32_t vfState;        /**< OUT: Virtualization state bitmap.*/
} cndevCardVfState_t;

/** MLU-Link types.*/
typedef enum {
	PORT_WORK_MODE_UNINITIALIZED = 0,       /**< Port is uninitialized.*/
	PORT_WORK_MODE_ALL_SUPPORT = 1,         /**< Port supports all modes.*/
	PORT_WORK_MODE_MLULINK = 2,             /**< Port supports MLU-Link mode.*/
	PORT_WORK_MODE_MLULINK_OVER_ROCE = 3,   /**< Port supports MLU-Link over RoCE mode.*/
} cndevPortModeEnum_t;

/** MLU-Link port mode information.*/
typedef struct {
	__int32_t version;                /**< IN: API version.*/
	cndevPortModeEnum_t mode;         /**< OUT: The current work mode of MLU-Link port.*/
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
	__int32_t version;                      /**< IN: API version.*/
	cndevMLULinkOverRoCEFieldEnum_t field;  /**< IN: The field name of MLU-Link over RoCE port.*/
	__uint32_t value;                       /**< OUT: The value of field name.*/
} cndevMLULinkOverRoCECtrl_t;

/** TinyCore utilization.*/
typedef struct {
	__int32_t version;		/**< IN: API version.*/
	__int32_t tinyCoreCount;	/**< OUT: The count of tiny cores.*/
	__int32_t tinyCoreUtilization[CNDEV_TINYCOREMAXCOUNT];	/**< OUT: The utilization of each tiny core.
								  * The valid array index range is [0, ``tinyCoreCount`` - 1].
								  */
} cndevTinyCoreUtilization_t;

/** Device OS memory information.*/
typedef struct {
	__uint32_t deviceSystemMemoryTotal;   /**< OUT: Device system total memory, unit: KB.*/
	__uint32_t deviceSystemMemoryUsed;    /**< OUT: Device system used memory, unit: KB.*/
	__uint32_t reserved[8];               /**< Reserved.*/
} cndevDeviceOsMemoryInfo_t;

/** Chip ID information.*/
typedef struct {
	__int32_t version;     /**< IN: API version.*/
	__uint8_t chipId;      /**< OUT: Chip ID.*/
} cndevChipId_t;

/** Frequency status.*/
typedef struct {
	__int32_t version;                /**< IN: API version.*/
	__uint8_t mluFrequencyLockStatus; /**< OUT: MLU frequency lock status.*/
} cndevMLUFrequencyStatus_t;

/** MLU-Link port IP information.*/
typedef struct {
	__int32_t version;             /**< IN: API version.*/
	__uint8_t ipVersion;           /**< OUT: IP version: IPV4 or IPV6.*/
	char ip[CNDEV_IP_ADDRESS_LEN]; /**< OUT: IP address of the MLU-Link port.*/
} cndevMLULinkPortIP_t;

/** CRC error information.*/
typedef struct {
	__int32_t version;                   /**< IN: API version.*/
	__uint64_t die2dieCRCError;          /**< OUT: The count of D2D CRC errors.*/
	__uint64_t die2dieCRCErrorOverflow;  /**< OUT: The count of D2D CRC error overflows.*/
} cndevCRCInfo_t;

/** DDR information.*/
typedef struct {
	__int32_t version;            /**< IN: API version.*/
	__uint32_t dataWidth;         /**< OUT: DDR data width.*/
	__uint32_t bandWidth;         /**< OUT: DDR band width, unit: GB/s.*/
	__uint32_t bandWidthDecimal;  /**< OUT: The decimal of DDR band width, unit: GB/s.*/
} cndevDDRInfo_t;

/** Process utilization.*/
typedef struct {
	__int32_t version;      /**< IN: API version.*/
	__uint32_t pid;         /**< OUT: Process ID.*/
	__uint32_t ipuUtil;     /**< OUT: The MLU utilization of the process.*/
	__uint32_t jpuUtil;     /**< OUT: The JPU utilization of the process.*/
	__uint32_t vpuDecUtil;  /**< OUT: The VPU decoder utilization of the process.*/
	__uint32_t vpuEncUtil;  /**< OUT: The VPU encoder utilization of the process.*/
	__uint32_t memUtil;     /**< OUT: The memory utilization of the process.*/
} cndevProcessUtilization_t;

/** Information about process average utilization.*/
typedef struct {
	__uint32_t pid;         /**< OUT: Process ID.*/
	__uint32_t ipuUtil;     /**< OUT: The MLU average utilization of the process.*/
	__uint32_t jpuUtil;     /**< OUT: The JPU average utilization of the process.*/
	__uint32_t vpuDecUtil;  /**< OUT: The VPU decoder average utilization of the process.*/
	__uint32_t vpuEncUtil;  /**< OUT: The VPU encoder average utilization of the process.*/
	__uint32_t memUtil;     /**< OUT: The memory utilization of the process.*/
	__uint64_t timestamp;   /**< OUT: The timestamp of the interface call for the device since 1970 (in microseconds).*/
} cndevProcessUtilizationInfo_t;

/** Power management limitations range information.*/
typedef struct {
	__int32_t version;                /**< IN: API version.*/
	__uint16_t minPowerLimit;         /**< OUT: Minimum power setting, unit: W.*/
	__uint16_t minPowerLimitDecimal;  /**< Reserved. Decimal part is not supported.*/
	__uint16_t maxPowerLimit;         /**< OUT: Maximum power setting, unit: W.*/
	__uint16_t maxPowerLimitDecimal;  /**< Reserved. Decimal part is not supported.*/
} cndevPowerManagementLimitationRange_t;

/** Power management limitations information.*/
typedef struct {
	__int32_t version;                /**< IN: API version.*/
	__uint16_t powerLimit;            /**< OUT: Power management limitations, unit: W.*/
	__uint16_t powerLimitDecimal;     /**< OUT: Reserved. Decimal part is not supported.*/
} cndevPowerManagementLimitation_t;

/** Over temperature information.*/
typedef struct {
	__int32_t version;		/**< IN: API version.*/
	__uint32_t powerOffCounter;	/**< OUT: The power-off times because of over-temperature, from loading driver to the current moment.*/
	__uint32_t underClockCounter;	/**< OUT: The underclocking times because of over-temperature, from loading driver to the current moment.*/
	__int16_t powerOffTemp;		/**< OUT: Over temperature causing poweroff will record the current temperature of IPU, the powerOffTemp
					  * is the highest value in the records, unit: ℃.
					  */
	__int16_t underClockTemp;	/**< OUT: Over temperature causing underClock will record the current temperature of IPU, the underClockTemp
					  * is the highest value in the records, unit: ℃.
					  */
} cndevOverTemperatureInfo_t;

/** Performance throttle reason information.*/
typedef struct {
	__int32_t version;                          /**< IN: API version.*/
	cndevEnableStatusEnum_t tdp;                /**< OUT: Throttle reason by TDP.*/
	cndevEnableStatusEnum_t powerSetting;       /**< OUT: Throttle reason by setting power capping.*/
	cndevEnableStatusEnum_t clockSetting;       /**< OUT: Throttle reason by setting max frequency.*/
	cndevEnableStatusEnum_t frequencyLocked;    /**< OUT: Throttle reason by setting locked frequency.*/
	cndevEnableStatusEnum_t powerBrake;         /**< OUT: Throttle reason by power brake.*/
	cndevEnableStatusEnum_t thermalSlowdown;    /**< OUT: Throttle reason by thermal slowdown.*/
} cndevPerformanceThrottleReason_t;

/** Compute mode types.*/
typedef enum {
	CNDEV_COMPUTEMODE_DEFAULT = 0,      /**< Default mode means that multiple contexts are allowed per device.*/
	CNDEV_COMPUTEMODE_EXCLUSIVE = 1,    /**< Exclusive mode means that only one context is allowed per device.*/
	CNDEV_COMPUTEMODE_PROHIBITED = 2,   /**< Prohibited mode means that no contexts are allowed per device.*/
	CNDEV_COMPUTEMODE_COUNT = 0xFF,     /**< The count of compute mode types.*/
} cndevComputeModeEnum_t;

/** Compute mode information.*/
typedef struct {
	__int32_t version;            /**< IN: API version.*/
	cndevComputeModeEnum_t mode;  /**< IN/OUT: The compute mode of the device.*/
} cndevComputeMode_t;

/** PCIe replay information.*/
typedef struct {
	__int32_t version;        /**< IN: API version.*/
	__uint32_t counter;       /**< OUT: The count of PCIe replays.*/
} cndevPcieReplayCounter_t;

/** SR-IOV mode information.*/
typedef struct {
	__int32_t version;                  /**< IN: API version.*/
	cndevEnableStatusEnum_t sriovMode;  /**< IN/OUT: The switch of SR-IOV mode.*/
} cndevSRIOVMode_t;

/** MIM mode information.*/
typedef struct {
	__int32_t version;                  /**< IN: API version.*/
	cndevEnableStatusEnum_t mimMode;    /**< IN/OUT: The switch of MIM mode.*/
} cndevMimMode_t;

/** MIM instance placement information.*/
typedef struct {
	__int32_t version;                  /**< IN: API version.*/
	__int32_t start;                    /**< IN/OUT: The start placement of MIM instance.*/
	__int32_t size;                     /**< IN/OUT: The size of MIM instance.*/
} cndevMluInstancePlacement_t;

/** MIM profile information.*/
typedef struct {
	__int32_t version;                                          /**< IN: API version.*/
	__int32_t profileId;                                        /**< OUT: MIM instance profile ID.*/
	__int32_t mluCoreCount;                                     /**< OUT: The MIM core count of profile.*/
	__int32_t vpuCount;                                         /**< OUT: The VPU count of profile.*/
	__int32_t jpuCount;                                         /**< OUT: The JPU count of profile.*/
	__int32_t gdmaCount;                                        /**< OUT: The GDMA count of profile.*/
	__int32_t memorySize;                                       /**< OUT: The memory size of profile.*/
	char name[CNDEV_MLUINSTANCE_PROFILE_NAME_BUFFER_SIZE];      /**< OUT: The name of profile.*/
} cndevMluInstanceProfileInfo_t;

/** MIM instance information.*/
typedef struct {
	__int32_t version;                                                          /**< IN: API version.*/
	cndevDevice_t parentDevice;                                                 /**< OUT: Parent device.*/
	__int32_t instanceId;                                                       /**< OUT: MIM instance Id.*/
	__int32_t profileId;                                                        /**< OUT: MIM instance created base on the profile.*/
	__uint8_t uuid[CNDEV_UUID_SIZE];                                            /**< OUT: The UUID of MIM instance.*/
	__int32_t domain;                                                           /**< OUT: The domain ID of MIM instance.*/
	__int32_t bus;                                                              /**< OUT: The bus ID of MIM instance.*/
	__int32_t device;                                                           /**< OUT: The device ID of MIM instance.*/
	__int32_t function;                                                         /**< OUT: The function ID of MIM instance.*/
	cndevMluInstancePlacement_t placement;                                      /**< OUT: The placement information of MIM instance.*/
	__int32_t mluCoreCount;                                                     /**< OUT: The MLU core count of MIM instance.*/
	__int32_t vpuCount;                                                         /**< OUT: The VPU count of MIM instance.*/
	__int32_t jpuCount;                                                         /**< OUT: The JPU count of MIM instance.*/
	__int32_t gdmaCount;                                                        /**< OUT: The GDMA count of MIM instance.*/
	__int32_t memorySize;                                                       /**< OUT: The memory size of MIM instance.*/
	char profileName[CNDEV_MLUINSTANCE_PROFILE_NAME_BUFFER_SIZE];               /**< OUT: The name of MIM profile.*/
	char devNodeName[CNDEV_MLUINSTANCE_DEVICE_NODE_NAME_BUFFER_SIZE];           /**< OUT: The device node of MIM instance.*/
	char ipcmDevNodeName[CNDEV_MLUINSTANCE_DEVICE_NODE_NAME_BUFFER_SIZE];       /**< OUT: The ipcm node of MIM instance.*/
	char instanceName[CNDEV_MLUINSTANCE_DEVICE_NODE_NAME_BUFFER_SIZE];          /**< OUT: The name of MIM instance.*/
	__uint32_t reserved[128];                                                   /**< Reserved.*/
} cndevMluInstanceInfo_t;

/** Parity error information.*/
typedef struct {
	__int32_t version;  /**< IN: API version.*/
	__int32_t counter;  /**< OUT: The count of parity errors.*/
} cndevParityError_t;

/** Docker parameter information.*/
typedef struct {
	__int32_t version;                          /**< IN: API version.*/
	char dockerParam[CNDEV_NAME_BUFFER_SIZE];   /**< OUT: The parameter of the running docker.*/
} cndevDockerParam_t;

/** sMLU mode information.*/
typedef struct {
	__int32_t version;                /**< IN: API version.*/
	cndevEnableStatusEnum_t smluMode; /**< IN/OUT: The switch of sMLU mode.*/
} cndevSMLUMode_t;

/** sMLU profile mluQuota and memorySize information for creating new profile.*/
typedef struct {
	__int32_t version;      /**< IN: API version.*/
	__uint32_t mluQuota;    /**< IN/OUT: The utilization of sMLU profile.*/
	__uint64_t memorySize;  /**< IN/OUT: The memory size of sMLU profile, unit: B.*/
} cndevSMluSet_t;

/** sMLU array indices for mluQuota and memorySize.*/
typedef enum {
	CNDEV_SMLU_MAX = 0,     /**< sMLU maximum information.*/
	CNDEV_SMLU_USAGE = 1,   /**< sMLU usage information.*/
	CNDEV_SMLU_ITEM_COUNT,  /**< The count of sMLU information.*/
} cndevSMluItemEnum_t;

/** sMLU instance information.*/
typedef struct {
	__int32_t version;                                                    /**< IN: API version.*/
	__int32_t profileId;                                                  /**< OUT: sMLU instance created base on the profile.*/
	__int32_t instanceId;                                                 /**< OUT: sMLU instance ID.*/
	__uint8_t uuid[CNDEV_UUID_SIZE];                                      /**< OUT: The UUID of sMLU instance.*/
	__int32_t domain;                                                     /**< OUT: The domain of sMLU instance.*/
	__int32_t bus;                                                        /**< OUT: The bus of sMLU instance.*/
	__int32_t device;                                                     /**< OUT: The device of sMLU instance.*/
	__int32_t function;                                                   /**< OUT: The function of sMLU instance.*/
	char profileName[CNDEV_MLUINSTANCE_PROFILE_NAME_BUFFER_SIZE];         /**< OUT: The name of sMLU profile.*/
	char instanceName[CNDEV_MLUINSTANCE_PROFILE_NAME_BUFFER_SIZE];        /**< OUT: The name of sMLU instance.*/
	char devNodeName[CNDEV_MLUINSTANCE_DEVICE_NODE_NAME_BUFFER_SIZE];     /**< OUT: The device node of sMLU instance.*/
	__uint32_t mluQuota[CNDEV_SMLU_ITEM_COUNT];                           /**< OUT: The utilization of sMLU instance.*/
	__uint64_t memorySize[CNDEV_SMLU_ITEM_COUNT];                         /**< OUT: The memory size of sMLU instance, unit: B.*/
	__uint32_t reserved[128];                                             /**< Reserved.*/
} cndevSMluInfo_t;

/** sMLU instance ID information.*/
typedef struct {
	__int32_t version;          /**< IN: API version.*/
	__int32_t count;            /**< OUT: The profile count of sMLU.*/
	__int32_t profileId[128];   /**< OUT: The profile ID information of sMLU.*/
} cndevSMluProfileIdInfo_t;

/** sMLU profile information.*/
typedef struct {
	__int32_t version;	/**< IN: API version.*/
	__int32_t profileId;	/**< OUT: Profile ID.*/
	char name[CNDEV_MLUINSTANCE_PROFILE_NAME_BUFFER_SIZE];	/**< OUT: Profile name.*/
	__uint32_t totalCapacity;	/**< OUT: The total count of sMLU instances that were created based on the profile.*/
	__uint32_t remainCapacity;	/**< OUT: The remain count of sMLU instances that were created based on the profile.*/
	__uint32_t mluQuota[CNDEV_SMLU_ITEM_COUNT];	/**< OUT: MluQuota[0] is the utilization of sMLU profile, mluQuota[1] is reserved.*/
	__uint64_t memorySize[CNDEV_SMLU_ITEM_COUNT];	/**< OUT: MemorySize[0] is the memory size of sMLU profile, unit: B,
							  * memorySize[1] is reserved.
							  */
} cndevSMluProfileInfo_t;

/** Memory ECC information.*/
typedef struct {
	__int32_t version;			/**< IN: API version.*/
	__uint32_t volatileSramEccSbeCounter;	/**< OUT: The count of volatile SRAM ECC SBE.
						  * This number is accumulated count from loading MLU Driver to the current time.
						  */
	__uint32_t volatileSramEccDbeCounter;	/**< OUT: The count of volatile SRAM ECC DBE.
						  * This number is accumulated count from loading MLU Driver to the current time.
						  */
	__uint32_t volatileSramEccParityCounter;	/**< OUT: The count of volatile SRAM ECC PARITY.
							  * This number is accumulated count from loading MLU Driver to the current time.
							  */
	__uint32_t volatileDramEccSbeCounter;	/**< OUT: The count of volatile DRAM ECC SBE.
						  * This number is accumulated count from loading MLU Driver to the current time.
						  */
	__uint32_t volatileDramEccDbeCounter;	/**< OUT: The count of volatile DRAM ECC DBE.
						  * This number is accumulated count from loading MLU Driver to the current time.
						  */
	__uint64_t aggregateDramEccSbeCounter;	/**< OUT: The count of aggregate DRAM ECC SBE.
						  * This number is accumulated count from the manufacturing date to the current time.
						  */
	__uint64_t aggregateDramEccDbeCounter;	/**< OUT: The count of aggregate DRAM ECC DBE.
						  * This number is accumulated count from the manufacturing date to the current time.
						  */
	__uint64_t aggregateSramEccSbeCounter;	/**< OUT: The count of aggregate SRAM ECC SBE.
						  * This number is accumulated count from the manufacturing date to the current time.
						  */
	__uint64_t aggregateSramEccDbeCounter;	/**< OUT: The count of aggregate SRAM ECC DBE.
						  * This number is accumulated count from the manufacturing date to the current time.
						  */
	__uint64_t aggregateSramEccParityCounter;	/**< OUT: The count of aggregate SRAM ECC PARITY.
							  * This number is accumulated count from the manufacturing date to the current time.
							  */
	bool aggregateSramEccThresholdExceeded;	/**< OUT: The identifier that indicates whether the aggregate SRAM ECCs exceeded the threshold.*/
	__uint64_t reserved[11];		/**< Reserved.*/
} cndevMemEccCounter_t;

/** ECC types.*/
typedef enum {
	CNDEV_ECC_SBE    = 0,     /**< Single bit ECC error.*/
	CNDEV_ECC_DBE    = 1,     /**< Double bits ECC error.*/
	CNDEV_ECC_PARITY = 2,     /**< Parity ECC error.*/
	CNDEV_ECC_MAX_TYPE,       /**< The count of ECC error types.*/
} cndevMemEccTypeEnum_t;

/** SRAM ECC histogram information.*/
typedef struct {
	struct {
	__uint32_t max;                 /**< OUT: The count of instances on which ECC errors are greater than or equal to @p threshold.*/
	__uint32_t high;                /**< OUT: The count of instances on which ECC errors are equal to @p threshold - 1.*/
	__uint32_t partial;             /**< OUT: The count of instances on which ECC errors are between @p low and @p high.*/
	__uint32_t low;                 /**< OUT: The count of instances on which ECC error is equal to 1.*/
	__uint32_t threshold;           /**< OUT: The current ECC error threshold.*/
	__uint32_t defaultThreshold;    /**< OUT: The default ECC error threshold.*/
	__uint32_t reserved[8];         /**< Reserved.*/
	} histogram[CNDEV_ECC_MAX_TYPE];
} cndevSramEccHistogram_t;

typedef struct {
	__uint64_t total_size;            /**< OUT: Total BAR4 Memory in bytes.*/
	__uint64_t used_size;             /**< OUT: Used BAR4 Memory in bytes.*/
	__uint64_t free_size;             /**< OUT: Free BAR4 Memory in bytes.*/
} cndevBAR4Memory_t;

/** Clock types, all speeds are in MHz.*/
typedef enum {
	CNDEV_CLOCK_IPU = 0,              /**< IPU clock domain.*/
} cndevClockType_t;

/** Event information.*/
typedef struct {
	__uint32_t version;                           /**< IN: API version.*/
	cndevDevice_t device;                         /**< OUT: Specific device where the event occurred.*/
	__uint64_t eventData;                         /**< OUT: Stores Xid error data for the device.*/
	__uint64_t eventType;                         /**< OUT: Stores Xid type for the device.*/
	__uint64_t timestamp;                         /**< OUT: Stores Xid timestamp for the device since 1970 (in microseconds).*/
	__uint32_t computeInstanceId;                 /**< OUT: When MIM is enabled, the event is attributable to a CPU.*/
	__uint32_t mluInstanceId;                     /**< OUT: When MIM is enabled, the event is attributable to an MLU.*/
	__uint64_t reserved[8];                       /**< Reserved.*/
} cndevEventData_t;

/** Device voltage information.*/
typedef struct {
	__int16_t ipuCoreVoltage;   /**< OUT: The voltage of IPU core, unit mV.*/
	__int16_t socVoltage;       /**< OUT: The voltage of SoC, unit mV.*/
	__int16_t hbmVccVoltage;    /**< Reserved.*/
	__int16_t hbmVddVoltage;    /**< OUT: The voltage of DRAM, unit mV.*/
	__int16_t reserved[16];     /**< Reserved.*/
} cndevVoltageInfo_t;

/** Device current information.*/
typedef struct {
	__int32_t ipuCoreCurrent;   /**< OUT: The current of IPU core, unit mA.*/
	__int32_t socCurrent;       /**< OUT: The current of SoC, unit mA.*/
	__int32_t hbmCurrent;       /**< OUT: The current of DRAM, unit mA.*/
	__int32_t reserved[16];     /**< Reserved.*/
} cndevCurrentInfo_t;

/** The event handle data type.*/
typedef void *cndevEventHandle;

#define cndevEventTypeAll      0xffffffffffffffffULL
#define cndevEventTypeNone     0x0000000000000000ULL
#define cndevEventTypeXidError 0x0000000000000001ULL

/** Union to represent different types of Value.*/
typedef union {
	double              dVal;         /**< Double value.*/
	unsigned int        uiVal;        /**< Unsigned int value.*/
	unsigned long       ulVal;        /**< Unsigned long value.*/
	unsigned long long  ullVal;       /**< Unsigned long long value.*/
	int                 siVal;        /**< Signed int value.*/
	long                slVal;        /**< Signed long value.*/
	long long           sllVal;       /**< Signed long long value.*/
} cndevValue_t;

typedef enum {
	CNDEV_VALUE_TYPE_DOUBLE = 0,               /**< Retrieved value type is double.*/
	CNDEV_VALUE_TYPE_UNSIGNED_INT = 1,         /**< Retrieved value type is unsigned int.*/
	CNDEV_VALUE_TYPE_UNSIGNED_LONG = 2,        /**< Retrieved value type is unsigned long.*/
	CNDEV_VALUE_TYPE_UNSIGNED_LONG_LONG = 3,   /**< Retrieved value type is unsigned long long.*/
	CNDEV_VALUE_TYPE_INT = 4,                  /**< Retrieved value type is int.*/
	CNDEV_VALUE_TYPE_LONG = 5,                 /**< Retrieved value type is long.*/
	CNDEV_VALUE_TYPE_LONG_LONG = 6,            /**< Retrieved value type is long long.*/
} cndevFieldValuesTypeNum_t;

typedef cndevFieldValuesTypeNum_t cndevFieldVaulesTypeNum_t;

typedef enum {
	cndevFieldECCCurrent = 0,		/**< Current ECC mode.*/
	cndevFieldECCPending = 1,		/**< Pending ECC mode.*/
	cndevFieldSramECCSBEVolCount = 2,	/**< SRAM ECC SBE count after loading MLU Driver.*/
	cndevFieldSramECCDBEVolCount = 3,	/**< SRAM ECC DBE count after loading MLU Driver.*/
	cndevFieldDramECCSBEVolCount = 4,	/**< DRAM ECC SBE count after loading MLU Driver.*/
	cndevFieldDramECCDBEVolCount = 5,	/**< DRAM ECC DBE count after loading MLU Driver.*/
	cndevFieldCurrentPciSpeed = 6,		/**< Current PCIe generation.*/
	cndevFieldCurrentPciWidth = 7,		/**< Current PCIe link width.*/
	cndevFieldMLULinkPortNum = 8,		/**< Device port number.*/
	cndevFieldMLULinkStatus = 9,		/**<
						  * Status of the MLU-Link.
						  * - 0 represents the status is link down.
						  * - 1 represents it is link up.
						  * - 2 represents it is LLDP packet received.
						  * - 3 represents it is link ready with a configured IP address.
						  */
	cndevFieldRetiredPagesSBE = 10,		/**< The retired page count due to SBE.*/
	cndevFieldRetiredPagesDBE = 11,		/**< The retired page count due to DBE.*/
	cndevFieldRetiredPagesPending = 12,	/**<
						  * The pending status of the retired page.
						  * - 0 represents the page is not pending.
						  * - 1 represents it is currently pending.
						  */
	cndevFieldRemapedRowCorrect = 13,	/**< The count of corrected rows.*/
	cndevFieldRemapedRowUnCorrect = 14,	/**< The count of uncorrected rows.*/
	cndevFieldRemapedRowPending = 15,	/**< The count of rows that are pending remapping.*/
	cndevFieldRemapedRowFailure = 16,	/**< The count of rows that failed to be remapped.*/
	cndevFieldPowerCurrent = 17,		/**< Current power, unit: W.*/
	cndevFieldPowerCapping = 18,		/**< Capping power, unit: W.*/
	cndevFieldPowerCappingMinLimit = 19,	/**< Minimum power limit, unit: W.*/
	cndevFieldPowerCappingMaxLimit = 20,	/**< Maximum power limit, unit: W.*/
	cndevFieldPowerCappingDefaultLimit = 21,/**< Default power limit, unit: W.*/
	cndevFieldPhysicalMemoryTotal = 22,	/**< MLU device physical total memory, unit: MB.*/
	cndevFieldPhysicalMemoryUsed = 23,	/**< MLU device physical used memory, unit: MB.*/
	cndevFieldVirtualMemoryTotal = 24,	/**< MLU device virtual total memory, unit: MB.*/
	cndevFieldVirtualMemoryUsed = 25,	/**< MLU device virtual used memory, unit: MB.*/
	cndevFieldDDRDataWidth = 26,		/**< MLU DDR data width.*/
	cndevFieldDDRBandWidth = 27,		/**< MLU DDR band width, unit: GB/s.*/
	cndevFieldComputeMode = 28,		/**<
						  * The compute mode of the device.
						  * 0 represents default mode.
						  * 1 represents exclusive mode.
						  * 2 represents prohibited mode.
						  */
	cndevFieldPcieReplay = 29,			/**< The count of PCIe replays.*/
	cndevFieldOverTemperaturePowerOffTemp = 30,	/**< Over-temperature power-off temperature, unit: ℃.*/
	cndevFieldOverTemperaturePowerOffTimes = 31,	/**< Over-temperature power-off times.*/
	cndevFieldOverTemperatureUnderClockTemp = 32,	/**< Over-temperature under clocking temperature, unit: ℃.*/
	cndevFieldOverTemperatureUnderClockTimes = 33,	/**< Over-temperature under clocking temperature times, unit: ℃.*/
	cndevFieldTemperatureBoard = 34,		/**< Board temperature, unit: ℃.*/
	cndevFieldTemperatureCluster = 35,		/**< Cluster temperature, unit: ℃.*/
	cndevFieldTemperatureMemoryDie = 36,		/**< Memory die temperature, unit: ℃.*/
	cndevFieldTemperatureChip = 37,			/**< Chip temperature, unit: ℃.*/
	cndevFieldTemperatureAirIn = 38,		/**< Air inlet temperature, unit: ℃.*/
	cndevFieldTemperatureAirOut = 39,		/**< Air outlet temperature, unit: ℃.*/
	cndevFieldTemperatureMemory = 40,		/**< Memory temperature, unit: ℃.*/
	cndevFieldTemperatureVI = 41,			/**< Video input temperature, unit: ℃.*/
	cndevFieldTemperatureCPU = 42,			/**< Device CPU temperature, unit: ℃.*/
	cndevFieldTemperatureISP = 43,			/**< ISP temperature, unit: ℃.*/
	cndevFieldCoreAverageUtilization = 44,		/**< MLU average core utilization.*/
	cndevFieldCoreUtilization = 45,			/**< MLU core utilization.*/
	cndevFieldEncoderCount = 46,			/**< Video encoder count.*/
	cndevFieldEncoderUtilization = 47,		/**< Video encoder utilization.*/
	cndevFieldDecoderCount = 48,			/**< Video decoder count.*/
	cndevFieldDecoderUtilization = 49,		/**< Video decoder utilization.*/
	cndevFieldImageCodecCount = 50,			/**< Image codec count.*/
	cndevFieldImageCodecUtilization = 51,		/**< Image codec utilization.*/
	cndevFieldTemperatureSlowDown = 52,		/**< Device slowdown temperature.*/
	cndevFieldTemperatureShutDown = 53,		/**< Device shutdown temperature.*/
	cndevFieldReservedMemory = 54,			/**< MLU device reserved memory, unit: MB.*/
	cndevFieldGlobalMemory = 55,			/**< MLU device global memory, unit: MB.*/
	cndevFieldDramECCSBEAggregateCount = 56,	/**< DRAM ECC SBE count after manufacturing date.*/
	cndevFieldDramECCDBEAggregateCount = 57,	/**< DRAM ECC DBE count after manufacturing date.*/
	cndevFieldSramECCSBEAggregateCount = 58,	/**< SRAM ECC SBE count after manufacturing date.*/
	cndevFieldSramECCDBEAggregateCount = 59,	/**< SRAM ECC DBE count after manufacturing date.*/
	cndevFieldSramECCParityAggregateCount = 60,	/**< SRAM ECC PARITY count after manufacturing date.*/
	cndevFieldSramECCParityVolCount = 61,		/**< SRAM ECC PARITY count after loading MLU Driver.*/
	cndevFieldOAMModuleId = 62,			/**< MLU device module ID on an OAM machine.*/
	cndevFieldMLULinkTxByte = 63,			/**< The total number of bytes of all packets transmitted with no error.*/
	cndevFieldMLULinkRxByte = 64,			/**< The total number of bytes of all packets received with no error.*/
	cndevFieldTensorAverageUtilization = 65,	/**< MLU average tensor utilization.*/
	cndevFieldMLUUtilization = 66,			/**< MLU utilization.*/
	cndevFieldPCIeAuxiliaryPowerConnectorSenseState = 67,	/**< PCIe auxiliary power connector sense state.*/
	cndevFieldMLUMaxOperatingTemperature = 68,	/**< MLU maximum operating temperature, unit: ℃.*/
	cndevFieldMemMaxOperatingTemperature = 69,	/**< Memory maximum operating temperature, unit: ℃.*/
	cndevFieldMarginTemperature = 70,		/**< Thermal margin temperature, unit: ℃.*/
	cndevFieldTotalEnergyConsumption = 71,		/**<
							  * Total energy consumption for the MLU since the driver
							  * was last reloaded, unit: mJ.
							  */
	cndevFieldPowerViolation = 72,			/**<
							  * Total time that power violations cause the MLU Core to be below
							  * the maximum frequency or frequency cap, unit: ns.
							  */
	cndevFieldThermalViolation = 73,		/**<
							  * Total time that thermal violations cause the MLU Core to be below
							  * the maximum frequency or frequency cap, unit: ns.
							  */
	cndevFieldBoardLimitViolation = 74,		/**<
							  * Total time that the board limit causes the MLU Core to be below
							  * the maximum frequency or frequency cap, unit: ns.
							  */
	cndevFieldBaseClocksViolation = 75,		/**<
							  * Total time that any limit causes the MLU Core to be below
							  * the maximum frequency or frequency cap, unit: ns.
							  */
	cndevFieldNum = 200,				/**< The maximum ID of Field.*/
} cndevFieldIdNum_t;

typedef struct {
	cndevFieldIdNum_t fieldId;	/**< IN: ID of the CNDev field to retrieve. It must be set before any function call the uses this struct.*/
	cndevRet_t ret;			/**< OUT: Return code for retrieving this field.*/
	cndevValue_t value;	/**< OUT: Value for this field. It is only valid when @p ret is ::CNDEV_SUCCESS.*/
	cndevFieldValuesTypeNum_t valueType;	/**< OUT: Type of the value stored in value. It is only valid when @p ret is ::CNDEV_SUCCESS.*/
	long long latencyUsec;	/**< OUT: The time taken to update this field value (in microseconds).
				  * It is only valid when @p ret is ::CNDEV_SUCCESS.
				  */
	long long timestamp;	/**< OUT: CPU timestamp of this value since 1970 (in microseconds). It is only valid when @p ret is ::CNDEV_SUCCESS.*/
	unsigned int scopeId;	/**< IN: @p scopeId represents:
				  *- Link ID when used for getting MLU-Link status.
				  *- Core ID when used for getting MLU core utilization.
				  *- Memory die ID when used for getting memory die temperature.
				  *- Cluster ID when used for getting cluster temperature.
				  */
} cndevFieldValue_t;

typedef cndevFieldValue_t cndevFieldVaule_t;

/** The callback function for traverse topology.*/
typedef __int32_t (*CNDEV_TRAVERSE_CALLBACK)(cndevTopologyNode_t *current, void *userdata);

/** MPM metric identifiers.*/
typedef enum {
	CNDEV_MPM_METRIC_IPU_UTIL                 = 1,   /**< MLU core average utilization.*/
	CNDEV_MPM_METRIC_MLU_UTIL                 = 2,   /**< MLU utilization.*/
	CNDEV_MPM_METRIC_TENSOR_UTIL              = 3,   /**< MLU tensor average utilization.*/
	CNDEV_MPM_METRIC_PCIE_TX_PER_SEC          = 20,  /**< PCIe traffic from this MLU, unit: MiB/sec.*/
	CNDEV_MPM_METRIC_PCIE_RX_PER_SEC          = 21,  /**< PCIe traffic to this MLU, unit: MiB/sec.*/
	CNDEV_MPM_METRIC_MLULINK_TOTAL_TX_PER_SEC = 60,  /**< MLULink write bandwidth for all links, unit: MiB/sec.*/
	CNDEV_MPM_METRIC_MLULINK_TOTAL_RX_PER_SEC = 61,  /**< MLULink read bandwidth for all links, unit: MiB/sec.*/
	CNDEV_MPM_METRIC_MLULINK_L0_TX_PER_SEC    = 62,  /**< MLULink write bandwidth for link 0, unit: MiB/sec.*/
	CNDEV_MPM_METRIC_MLULINK_L0_RX_PER_SEC    = 63,  /**< MLULink read bandwidth for link 0, unit: MiB/sec.*/
	CNDEV_MPM_METRIC_MLULINK_L1_TX_PER_SEC    = 64,  /**< MLULink write bandwidth for link 1, unit: MiB/sec.*/
	CNDEV_MPM_METRIC_MLULINK_L1_RX_PER_SEC    = 65,  /**< MLULink read bandwidth for link 1, unit: MiB/sec.*/
	CNDEV_MPM_METRIC_MLULINK_L2_TX_PER_SEC    = 66,  /**< MLULink write bandwidth for link 2, unit: MiB/sec.*/
	CNDEV_MPM_METRIC_MLULINK_L2_RX_PER_SEC    = 67,  /**< MLULink read bandwidth for link 2, unit: MiB/sec.*/
	CNDEV_MPM_METRIC_MLULINK_L3_TX_PER_SEC    = 68,  /**< MLULink write bandwidth for link 3, unit: MiB/sec.*/
	CNDEV_MPM_METRIC_MLULINK_L3_RX_PER_SEC    = 69,  /**< MLULink read bandwidth for link 3, unit: MiB/sec.*/
	CNDEV_MPM_METRIC_MLULINK_L4_TX_PER_SEC    = 70,  /**< MLULink write bandwidth for link 4, unit: MiB/sec.*/
	CNDEV_MPM_METRIC_MLULINK_L4_RX_PER_SEC    = 71,  /**< MLULink read bandwidth for link 4, unit: MiB/sec.*/
	CNDEV_MPM_METRIC_MLULINK_L5_TX_PER_SEC    = 72,  /**< MLULink write bandwidth for link 5, unit: MiB/sec.*/
	CNDEV_MPM_METRIC_MLULINK_L5_RX_PER_SEC    = 73,  /**< MLULink read bandwidth for link 5, unit: MiB/sec.*/
	CNDEV_MPM_METRIC_MLULINK_L6_TX_PER_SEC    = 74,  /**< MLULink write bandwidth for link 6, unit: MiB/sec.*/
	CNDEV_MPM_METRIC_MLULINK_L6_RX_PER_SEC    = 75,  /**< MLULink read bandwidth for link 6, unit: MiB/sec.*/
	CNDEV_MPM_METRIC_MLULINK_L7_TX_PER_SEC    = 76,  /**< MLULink write bandwidth for link 7, unit: MiB/sec.*/
	CNDEV_MPM_METRIC_MLULINK_L7_RX_PER_SEC    = 77,  /**< MLULink read bandwidth for link 7, unit: MiB/sec.*/
	CNDEV_MPM_METRIC_MLULINK_L8_TX_PER_SEC    = 78,  /**< MLULink write bandwidth for link 8, unit: MiB/sec.*/
	CNDEV_MPM_METRIC_MLULINK_L8_RX_PER_SEC    = 79,  /**< MLULink read bandwidth for link 8, unit: MiB/sec.*/
	CNDEV_MPM_METRIC_MLULINK_L9_TX_PER_SEC    = 80,  /**< MLULink write bandwidth for link 9, unit: MiB/sec.*/
	CNDEV_MPM_METRIC_MLULINK_L9_RX_PER_SEC    = 81,  /**< MLULink read bandwidth for link 9, unit: MiB/sec.*/
	CNDEV_MPM_METRIC_MLULINK_L10_TX_PER_SEC   = 82,  /**< MLULink write bandwidth for link 10, unit: MiB/sec.*/
	CNDEV_MPM_METRIC_MLULINK_L10_RX_PER_SEC   = 83,  /**< MLULink read bandwidth for link 10, unit: MiB/sec.*/
	CNDEV_MPM_METRIC_MLULINK_L11_TX_PER_SEC   = 84,  /**< MLULink write bandwidth for link 11, unit: MiB/sec.*/
	CNDEV_MPM_METRIC_MLULINK_L11_RX_PER_SEC   = 85,  /**< MLULink read bandwidth for link 11, unit: MiB/sec.*/
	CNDEV_MPM_METRIC_MLULINK_L12_TX_PER_SEC   = 86,  /**< MLULink write bandwidth for link 12, unit: MiB/sec.*/
	CNDEV_MPM_METRIC_MLULINK_L12_RX_PER_SEC   = 87,  /**< MLULink read bandwidth for link 12, unit: MiB/sec.*/
	CNDEV_MPM_METRIC_MLULINK_L13_TX_PER_SEC   = 88,  /**< MLULink write bandwidth for link 13, unit: MiB/sec.*/
	CNDEV_MPM_METRIC_MLULINK_L13_RX_PER_SEC   = 89,  /**< MLULink read bandwidth for link 13, unit: MiB/sec.*/
	CNDEV_MPM_METRIC_MLULINK_L14_TX_PER_SEC   = 90,  /**< MLULink write bandwidth for link 14, unit: MiB/sec.*/
	CNDEV_MPM_METRIC_MLULINK_L14_RX_PER_SEC   = 91,  /**< MLULink read bandwidth for link 14, unit: MiB/sec.*/
	CNDEV_MPM_METRIC_MLULINK_L15_TX_PER_SEC   = 92,  /**< MLULink write bandwidth for link 15, unit: MiB/sec.*/
	CNDEV_MPM_METRIC_MLULINK_L15_RX_PER_SEC   = 93,  /**< MLULink read bandwidth for link 15, unit: MiB/sec.*/
	CNDEV_MPM_METRIC_MLULINK_L16_TX_PER_SEC   = 94,  /**< MLULink write bandwidth for link 16, unit: MiB/sec.*/
	CNDEV_MPM_METRIC_MLULINK_L16_RX_PER_SEC   = 95,  /**< MLULink read bandwidth for link 16, unit: MiB/sec.*/
	CNDEV_MPM_METRIC_MLULINK_L17_TX_PER_SEC   = 96,  /**< MLULink write bandwidth for link 17, unit: MiB/sec.*/
	CNDEV_MPM_METRIC_MLULINK_L17_RX_PER_SEC   = 97,  /**< MLULink read bandwidth for link 17, unit: MiB/sec.*/
} cndevMpmMetricId_t;

#define CNDEV_MPM_METRIC_MAX 256 /**< MPM metric maximum value.
				   * Note that changing this should also change CNDEV_MPM_METRICS_GET_VERSION
				   * due to struct size change.
				   */

/** Handle to an allocated MPM sample allocated with ::cndevMpmSampleAlloc. Free this with ::cndevMpmSampleFree.*/
typedef void *cndevMpmSample_t;

/** MPM metric name and unit.*/
typedef struct {
	char *shortName;   /**< OUT: The short name of metric.*/
	char *longName;    /**< OUT: The long name of metric.*/
	char *unit;        /**< OUT: The unit of metric.*/
} cndevMpmMetricInfo_t;

/** MPM metric information.*/
typedef struct {
	cndevMpmMetricId_t metricId;        /**< IN: The ID of the metric to retrieve.*/
	cndevRet_t ret;                     /**< OUT: Status of this metric. If this is non-zero, then the value is not valid.*/
	double value;                       /**< OUT: Value of this metric. It is only valid when @p ret is ::CNDEV_SUCCESS.*/
	cndevMpmMetricInfo_t metricInfo;    /**< OUT: Metric name and unit. Those can be NULL if not defined.*/
} cndevMpmMetric_t;

/** MPM buffer information.*/
typedef struct {
	unsigned int version;                              /**< IN: Set to CNDEV_MPM_METRICS_GET_VERSION.*/
	unsigned int numMetrics;                           /**< IN: How many metrics to retrieve in metrics[].*/
	cndevMpmSample_t sample1;                          /**< IN: Sample buffer.*/
	cndevMpmSample_t sample2;                          /**< IN: Sample buffer.*/
	cndevMpmMetric_t metrics[CNDEV_MPM_METRIC_MAX];    /**< IN/OUT: Array of metrics. Set @p metricId on call.
							     * See @p ret and @p value on return.
							     */
} cndevMpmMetricsGet_t;

#define CNDEV_MPM_METRICS_GET_VERSION 1

/*************************************************************************
 * Initialization Management
 *************************************************************************/
// Group: Initialization Management
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
cndevRet_t cndevInit(__int32_t reserved);

// Group: Initialization Management
/**
 * @brief Completes exit work.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_UNKNOWN
 */
CNDEV_EXPORT
cndevRet_t cndevRelease(void);

/*************************************************************************
 * Error Code Management
 *************************************************************************/
// Group: Error Code Management
/**
 * @brief Returns the calling thread's last-error code.
 *
 * @return
 * - The value of the last error that occurred during the execution of this program.
 */
CNDEV_EXPORT
cndevRet_t cndevGetLastError(void);

// Group: Error Code Management
/**
 * @brief Gets the error message.
 *
 * This API takes an error ID as input and returns the corresponding error message if found.
 * If the error message cannot be found, it returns "Cndev_ERROR not found!".
 *
 * @param[in] errorId The error ID.
 *
 * @return
 * - The corresponding error message if found, otherwise "Cndev_ERROR not found!".
 */
CNDEV_EXPORT
const char *cndevGetErrorString(cndevRet_t errorId);

/******************************************************************************
 * Version Management
 ******************************************************************************/
// Group: Version Management
/**
 * @brief Gets the information of the MLU Driver version.
 *
 * @param[out] majorVersion The major version of MLU Driver version.
 * @param[out] minorVersion The minor version of MLU Driver version.
 * @param[out] buildVersion The build version of MLU Driver version.
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_NO_DEVICES
 */
CNDEV_EXPORT
cndevRet_t cndevGetDriverVersion(__uint32_t *majorVersion, __uint32_t *minorVersion, __uint32_t *buildVersion);

// Group: Version Management
/**
 * @brief Gets the information of the device's MCU version and MLU Driver version.
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

// Group: Version Management
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
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetLibVersion(cndevLibVersionInfo_t *libVerInfo);

// Group: Version Management
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
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetPCIeFirmwareVersion(cndevPCIeFirmwareVersion_t *version, cndevDevice_t device);

// Group: Version Management
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

/******************************************************************************
 * Device Management
 ******************************************************************************/
// Group: Device Management
/**
 * @brief Gets the amount of devices.
 *
 * @param[in,out] cardNum The pointer which stores the amount of devices after the function ends.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION
 */
CNDEV_EXPORT
cndevRet_t cndevGetDeviceCount(cndevCardInfo_t *cardNum);

// Group: Device Management
/**
 * @brief Gets the minor number of the device.
 *
 * @param[out] minor The pointer which stores the minor number of the device after the function ends.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT
 */
CNDEV_EXPORT
cndevRet_t cndevDeviceGetMinorNumber(__uint32_t *minor, cndevDevice_t device);

// Group: Device Management
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

// Group: Device Management
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
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetFrequencyInfo(cndevFrequencyInfo_t *freqInfo, cndevDevice_t device);

// Group: Device Management
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
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetTemperatureInfo(cndevTemperatureInfo_t *tempInfo, cndevDevice_t device);

// Group: Device Management
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
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetFanSpeedInfo(cndevFanSpeedInfo_t *fanInfo, cndevDevice_t device);

// Group: Device Management
/**
 * @brief Gets the information of the device's processes.
 *
 * @param[in,out] infoCount The size of the space which the user allocates for storing the information of processes. At the same time, it
 * is also a pointer to the size of a space that is used to store all information after the function ends.
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
cndevRet_t cndevGetProcessInfo(__uint32_t *infoCount, cndevProcessInfo_t *procInfo, cndevDevice_t device);

// Group: Device Management
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
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetCoreCount(cndevCardCoreCount_t *cardCoreCount, cndevDevice_t device);

// Group: Device Management
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
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetClusterCount(cndevCardClusterCount_t *clusCount, cndevDevice_t device);

// Group: Device Management
/**
 * @brief Gets the index of the device's name.
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

// Group: Device Management
/**
 * @brief Translates the index of a device's name to the string of the device's name.
 *
 * @param[in] cardName The index of a device's name.
 *
 * @return
 * - "Unknown" if the string of the device's name can't be found, otherwise the string of the device's name will be returned.
 */
CNDEV_EXPORT
const char *cndevGetCardNameString(cndevNameEnum_t cardName);

// Group: Device Management
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

// Group: Device Management
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

// Group: Device Management
/**
 * @brief Gets the PN (part number) of the device.
 *
 * @param[in,out] cardPN The pointer which stores the PN of the device after the function ends.
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

// Group: Device Management
/**
 * @brief Gets the healthy state of the device.
 *
 * @param[in,out] cardHealthState The pointer to the structure storing the HP of the device after the function ends. 1 means health, and 0 means sick.
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
cndevRet_t cndevGetCardHealthState(cndevCardHealthState_t *cardHealthState, cndevDevice_t device);

// Group: Device Management
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
cndevRet_t cndevGetCardHealthStateV2(cndevCardHealthStateV2_t *cardHealthState, cndevDevice_t device);

// Group: Device Management
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
cndevRet_t cndevGetCardHeartbeatCount(cndevCardHeartbeatCount_t *cardHeartbeatCount, cndevDevice_t device);

// Group: Device Management
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
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetFastAlloc(cndevFastAlloc_t *fastalloc, cndevDevice_t device);

// Group: Device Management
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
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetCodecTurbo(cndevCodecTurbo_t *codecTurbo, cndevDevice_t device);

// Group: Device Management
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
cndevRet_t cndevGetNUMANodeIdByDevId(cndevNUMANodeId_t *numaNodeId, cndevDevice_t device);

// Group: Device Management
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
cndevRet_t cndevGetChassisInfoV2(cndevChassisInfoV2_t *chassisinfo, cndevDevice_t device);

// Group: Device Management
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

// Group: Device Management
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

// Group: Device Management
/**
 * @brief Gets device OS memory usage information.
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

// Group: Device Management
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
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetChipId(cndevChipId_t *chipid, cndevDevice_t device);

// Group: Device Management
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

// Group: Device Management
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
 * - ::CNDEV_ERROR_NO_PERMISSION,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevUnlockMLUFrequency(cndevDevice_t device);

// Group: Device Management
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

// Group: Device Management
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
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetDDRInfo(cndevDDRInfo_t *ddrInfo, cndevDevice_t device);

// Group: Device Management
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

// Group: Device Management
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
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetPerformanceThrottleReason(cndevPerformanceThrottleReason_t *reason, cndevDevice_t device);

// Group: Device Management
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

// Group: Device Management
/**
 * @brief Sets the information of the device's compute mode.
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
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_NO_PERMISSION,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevSetComputeMode(cndevComputeMode_t *mode, cndevDevice_t device);

// Group: Device Management
/**
 * @brief Gets MLU device handle for the given index.
 *
 * @param[in] index The index of the target MLU.
 * @param[out] handle The MLU device handle.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_NOT_FOUND,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetDeviceHandleByIndex(__int32_t index, cndevDevice_t *handle);

// Group: Device Management
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
cndevRet_t cndevGetDevIdByBDF(__int32_t version, cndevDevice_t *dev, __uint32_t domain, __uint32_t bus, __uint32_t device,
					__uint32_t function);

// Group: Device Management
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
 * - ::CNDEV_ERROR_NOT_FOUND,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetDeviceHandleByPciBusId(const char *pciBusId, cndevDevice_t *handle);

// Group: Device Management
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

// Group: Device Management
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

// Group: Device Management
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
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetParityError(cndevParityError_t *error, cndevDevice_t handle);

// Group: Device Management
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
 * - ::CNDEV_ERROR_IN_USE,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetDockerParam(cndevDockerParam_t *param, cndevDevice_t device);

// Group: Device Management
/**
 * @brief Gets total available and used size of BAR4 memory. BAR4 is used to map the device memory.
 *
 * @param[out] bar4Memory The pointer which stores the information of the device's BAR4 memory after the function ends.
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

// Group: Device Management
/**
 * @brief Gets major and minor compute capability of the device.
 *
 * @param[out] major The pointer which stores the major compute capability of the device.
 * @param[out] minor The pointer which stores the minor compute capability of the device.
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
cndevRet_t cndevGetComputeCapability(__uint32_t *major, __uint32_t *minor, cndevDevice_t device);

// Group: Device Management
/**
 * @brief Gets the information of the device's application clock.
 *
 * @param[in] clockType Identify which clock domain to query.
 * @param[out] clockMHz The pointer which stores the current setting of application clock in MHz. 0 indicates that it has not been set.
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
cndevRet_t cndevGetApplicationsClock(cndevClockType_t clockType, __uint32_t *clockMHz, cndevDevice_t device);

// Group: Device Management
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
cndevRet_t cndevDeviceGetFieldValues(cndevDevice_t device, __int32_t valueCount, cndevFieldValue_t *value);

// Group: Device Management
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
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetVoltageInfo(cndevVoltageInfo_t *voltage, cndevDevice_t device);

// Group: Device Management
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
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetCurrentInfo(cndevCurrentInfo_t *current, cndevDevice_t device);

// Group: Device Management
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
 * - ::CNDEV_ERROR_TIMEOUT,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevResetDevice(cndevDevice_t device);

// Group: Device Management
/**
 * @brief Checks the peer-to-peer ability.
 *
 * @param[out] canPeerAble The pointer which stores the peer-to-peer ability between @p deviceSrc and @p deviceDst.
 * @param[in] deviceSrc The identifier of the source device.
 * @param[in] deviceDst The identifier of the destination device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetDeviceCanPeerAble(bool *canPeerAble, cndevDevice_t deviceSrc, cndevDevice_t deviceDst);

// Group: Device Management
/**
 * @brief Gets the private function.
 *
 * @param[in] name The name of the private function.
 * @param[out] func The pointer of the private function.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_NOT_FOUND
 */
CNDEV_EXPORT
cndevRet_t cndevGetExportFunction(const char *name, const void **func);


/******************************************************************************
 * PCIe Management
 ******************************************************************************/
// Group: PCIe Management
/**
 * @brief Gets the current PCI information.
 *
 * @param[in,out] currentPCIInfo The pointer to the structure storing the current PCI information
 *   after the function ends.
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

// Group: PCIe Management
/**
 * @brief Gets the max PCI information.
 *
 * @param[in,out] maxPCIInfo The pointer to the structure storing the max PCI information.
 *   after the function ends.
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
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetMaxPCIInfo(cndevPCIInfo_t *maxPCIInfo, cndevDevice_t device);

// Group: PCIe Management
/**
 * @brief Gets the device information of PCIe.
 *
 * @param[out] deviceInfo The pointer which stores the information of the device ID after the function ends.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNKNOWN
 */
CNDEV_EXPORT
cndevRet_t cndevGetPCIeInfoV2(cndevPCIeInfoV2_t *deviceInfo, cndevDevice_t device);

// Group: PCIe Management
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
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetPCIethroughputV2(cndevPCIethroughputV2_t *pciethroughput, cndevDevice_t device);

// Group: PCIe Management
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
cndevRet_t cndevGetLowestLinkSpeed(cndevLinkSpeed_t *linkspeed, cndevDevice_t device);

// Group: PCIe Management
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
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetPcieReplayCounter(cndevPcieReplayCounter_t *count, cndevDevice_t device);

/******************************************************************************
 * Topology Management
 ******************************************************************************/
// Group: Topology Management
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

// Group: Topology Management
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
cndevRet_t cndevClearCurrentThreadAffinity(__int32_t version, cndevDevice_t device);

// Group: Topology Management
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
cndevRet_t cndevSetCurrentThreadAffinity(__int32_t version, cndevDevice_t device);

// Group: Topology Management
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

// Group: Topology Management
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
cndevRet_t cndevTopologyGetNearestDevices(__int32_t version, cndevTopologyRelationshipEnum_t rel, __uint64_t *count,
				__uint64_t *deviceArray, cndevDevice_t device);

// Group: Topology Management
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
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetNUMANodeIdByTopologyNode(cndevNUMANodeId_t *numaNodeId, cndevTopologyNode_t *treeNode);

// Group: Topology Management
/**
 * @brief Gets two devices' relationship.
 *
 * @param[in] version The CNDev version.
 * @param[in] cpuId Specified CPU ID.
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
cndevRet_t cndevTopologyGetCpuRelatedDevices(__int32_t version, __int32_t cpuId, __uint64_t *count, __uint64_t *deviceArray);

// Group: Topology Management
/**
 * @brief Gets two nodes' relationship.
 *
 * @param[in,out] relationship The pointer which stores two devices' relationship.
 * @param[in] node1 The topology node.
 * @param[in] node2 The topology node.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_NOT_FOUND
 */
CNDEV_EXPORT
cndevRet_t cndevTopologyGetRelationshipByNode(cndevTopologyRelationship_t *relationship, cndevTopologyNode_t *node1,
				cndevTopologyNode_t *node2);

// Group: Topology Management
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
cndevRet_t cndevGetNodeByBDF(__int32_t version, cndevTopologyNode_t **treeNode, __uint32_t domain, __uint32_t bus,
					 __uint32_t device, __uint32_t function);

// Group: Topology Management
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
cndevRet_t cndevGetNodeByDevId(__int32_t version, cndevTopologyNode_t **treeNode, cndevDevice_t device);

// Group: Topology Management
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
cndevRet_t cndevTopologyGetVirtualRootNode(__int32_t version, cndevTopologyNode_t **root);

// Group: Topology Management
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
cndevRet_t cndevTopologyTraverseTree(__int32_t version, CNDEV_TRAVERSE_CALLBACK callback, void *userdata);

// Group: Topology Management
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

// Group: Topology Management
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
cndevRet_t cndevGetNodeByDeviceName(__int32_t version, __int32_t *count, cndevTopologyNode_t *nodeArray, const char *deviceName);

/******************************************************************************
 * MLULink Management
 ******************************************************************************/
// Group: MLULink Management
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
cndevRet_t cndevGetMLULinkVersion(cndevMLULinkVersion_t *version, cndevDevice_t device, __int32_t link);

// Group: MLULink Management
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
 * - ::CNDEV_ERROR_INVALID_LINK,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetMLULinkStatusV2(cndevMLULinkStatusV2_t *status, cndevDevice_t device, __int32_t link);

// Group: MLULink Management
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
 * - ::CNDEV_ERROR_INVALID_LINK,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetOpticalInfo(cndevOpticalInfo_t *optical, cndevDevice_t device, __int32_t link);

// Group: MLULink Management
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
cndevRet_t cndevGetMLULinkSpeedInfo(cndevMLULinkSpeed_t *speed, cndevDevice_t device, __int32_t link);

// Group: MLULink Management
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
 * - ::CNDEV_ERROR_INVALID_LINK,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetMLULinkCapability(cndevMLULinkCapability_t *capability, cndevDevice_t device, __int32_t link);

// Group: MLULink Management
/**
 * @brief Gets the MLU-Link count information.
 *
 * @par Deprecated
 * - ::cndevGetMLULinkCounter is deprecated and will be removed in future release. Use ::cndevGetMLULinkBasicCounter,
 *   ::cndevGetMLULinkCongestionCtrlCounter, ::cndevGetMLULinkErrorCounter, ::cndevGetMLULinkFlowCtrlCounter,
 *   ::cndevGetMLULinkEventCounter, and ::cndevGetMLULinkTaskStatsCounter instead.
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
 */
CNDEV_EXPORT
cndevRet_t cndevGetMLULinkCounter(cndevMLULinkCounter_t *count, cndevDevice_t device, __int32_t link);

// Group: MLULink Management
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
cndevRet_t cndevGetMLULinkCounterExt(cndevMLULinkCounterExt_t *count, cndevDevice_t device, __int32_t link);

// Group: MLULink Management
/**
 * @brief Resets the counters of the MLU-Link.
 *
 * @par Deprecated
 * - ::cndevResetMLULinkCounter is deprecated and will be removed in future release.
 *   Use ::cndevResetMLULinkAllCounters instead.
 *
 * @param[in] setcount The pointer to the structure storing the MLU-Link count.
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
cndevRet_t cndevResetMLULinkCounter(cndevMLULinkSetCounter_t *setcount, cndevDevice_t device, __int32_t link);

// Group: MLULink Management
/**
 * @brief Resets the MLU-Link count.
 *
 * @param[in] device The identifier of the target device.
 * @param[in] link The count of the port which the user selects, starting from 0.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_INVALID_LINK,
 * - ::CNDEV_ERROR_IN_PROBLEM
 *
 * @note
 * - This function will reset all counters to zero. For the list of counters that are reset, see
 *   ::cndevMLULinkBasicCounter_t, ::cndevMLULinkCongestionCtrlCounter_t, ::cndevMLULinkErrorCounter_t,
 *   ::cndevMLULinkFlowCtrlCounter_t, and ::cndevMLULinkEventCounter_t.
 */
CNDEV_EXPORT
cndevRet_t cndevResetMLULinkAllCounters(cndevDevice_t device, __int32_t link);

// Group: MLULink Management
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
 * - ::CNDEV_ERROR_INVALID_LINK,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetMLULinkBasicCounter(cndevMLULinkBasicCounter_t *counter, cndevDevice_t device, __int32_t link);

// Group: MLULink Management
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
 * - ::CNDEV_ERROR_INVALID_LINK,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetMLULinkCongestionCtrlCounter(cndevMLULinkCongestionCtrlCounter_t *counter, cndevDevice_t device, __int32_t link);

// Group: MLULink Management
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
 * - ::CNDEV_ERROR_INVALID_LINK,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetMLULinkErrorCounter(cndevMLULinkErrorCounter_t *counter, cndevDevice_t device, __int32_t link);

// Group: MLULink Management
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
 * - ::CNDEV_ERROR_INVALID_LINK,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetMLULinkFlowCtrlCounter(cndevMLULinkFlowCtrlCounter_t *counter, cndevDevice_t device, __int32_t link);

// Group: MLULink Management
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
 * - ::CNDEV_ERROR_INVALID_LINK,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetMLULinkEventCounter(cndevMLULinkEventCounter_t *counter, cndevDevice_t device, __int32_t link);

// Group: MLULink Management
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
 * - ::CNDEV_ERROR_INVALID_LINK,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetMLULinkTaskStatsCounter(cndevMLULinkTaskStatsCounter_t *counter, cndevDevice_t device, __int32_t link);

// Group: MLULink Management
/**
 * @brief Sets an MLU-Link state. This will take effect after resetting the device.
 *
 * @param[in] device The identifier of the target device.
 * @param[in] link The count of the ports which the user selects, starting from 0.
 * @param[in] enable The target state of the port which the user wants to set.
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
cndevRet_t cndevSetMLULinkState(cndevDevice_t device, __int32_t link, bool enable);

// Group: MLULink Management
/**
 * @brief Gets an MLU-Link state.
 *
 * @param[out] ib_cmd_info The inbound command info of the port.
 * @param[out] ob_cmd_info The outbound command info of the port.
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
cndevRet_t cndevGetMLULinkState(__int32_t *ib_cmd_info, __int32_t *ob_cmd_info, cndevDevice_t device, __int32_t link);

// Group: MLULink Management
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
 * - ::CNDEV_ERROR_INVALID_LINK,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetMLULinkRemoteInfo(cndevMLULinkRemoteInfo_t *remoteinfo, cndevDevice_t device, __int32_t link);

// Group: MLULink Management
/**
 * @brief Gets the management IP address of the MLU-Link remote host.
 *
 * @param[out] mgmt_addr The pointer to the structure storing the management IP address of the MLU-Link remote host.
 * @param[in] device The identifier of the target device.
 * @param[in] link The count of the ports which the user selects, starting from 0.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_INVALID_LINK,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetRemoteHostMgmtAddr(cndevMLULinkMgmtAddr_t *mgmt_addr, cndevDevice_t device, __int32_t link);

// Group: MLULink Management
/**
 * @brief Gets the serial number of the MLU-Link remote host.
 *
 * @param[out] host_sn The pointer to the structure storing the serial number of the MLU-Link remote host.
 * @param[in] device The identifier of the target device.
 * @param[in] link The count of the ports which the user selects, starting from 0.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_INVALID_LINK,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetRemoteHostSN(cndevMLULinkHostSN_t *host_sn, cndevDevice_t device, __int32_t link);

// Group: MLULink Management
/**
 * @brief Gets the physical port index name of the scale out MLU-Link.
 *
 * @param[out] ppi The pointer to the structure storing the physical port index name of the scale out MLU-Link.
 * @param[in] device The identifier of the target device.
 * @param[in] link The count of the ports which the user selects, starting from 0.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_INVALID_LINK,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetMLULinkPPI(cndevMLULinkPPI_t *ppi, cndevDevice_t device, __int32_t link);

// Group: MLULink Management
/**
 * @brief Gets the OAM port name of the MLU-Link.
 *
 * @param[out] opn The pointer to the structure storing the OAM port name of the MLU-Link.
 * @param[in] device The identifier of the target device.
 * @param[in] link The count of the ports which the user selects, starting from 0.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_INVALID_LINK,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetMLULinkOPN(cndevMLULinkOPN_t *opn, cndevDevice_t device, __int32_t link);

// Group: MLULink Management
/**
 * @brief Gets the MLU-Link devices' SN.
 *
 * @param[in,out] devinfo The pointer to the structure storing the MLU-Link devices' SN.
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
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetMLULinkDevSN(cndevMLULinkDevSN_t *devinfo, cndevDevice_t device);

// Group: MLULink Management
/**
 * @brief Gets device MLU-Link port mode.
 *
 * @param[in,out] mode The pointer which stores the mode of the device.
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
cndevRet_t cndevGetMLULinkPortMode(cndevMLULinkPortMode_t *mode, cndevDevice_t device, __int32_t port);

// Group: MLULink Management
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
 * - ::CNDEV_ERROR_INVALID_LINK,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetMLULinkOverRoCECtrl(cndevMLULinkOverRoCECtrl_t *ctrl, cndevDevice_t device, __int32_t port);

// Group: MLULink Management
/**
 * @brief Gets device port number.
 *
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - Returns the port count of the device.
 */
CNDEV_EXPORT
__int32_t cndevGetMLULinkPortNumber(cndevDevice_t device);

// Group: MLULink Management
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
 * - ::CNDEV_ERROR_INVALID_LINK,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetMLULinkPortIP(cndevMLULinkPortIP_t *ip, cndevDevice_t device, __int32_t port);

/******************************************************************************
 * ECC Management
 ******************************************************************************/
// Group: ECC Management
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

// Group: ECC Management
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
 * - ::CNDEV_ERROR_NO_PERMISSION,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevSetEccMode(cndevEccMode_t *eccMode, cndevDevice_t device);

// Group: ECC Management
/**
 * @brief Gets the ECC mode of the device.
 *
 * @param[in,out] currentMode The pointer which stores the current ECC mode of the device.
 * @param[in,out] pendingMode The pointer which stores the pending ECC mode of the device.
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
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetEccMode(cndevEccMode_t *currentMode, cndevEccMode_t *pendingMode, cndevDevice_t device);

// Group: ECC Management
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
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetMemEccCounter(cndevMemEccCounter_t *eccCounter, cndevDevice_t device);

// Group: ECC Management
/**
 * @brief Gets the SRAM ECC histogram of the device.
 *
 * @param[out] eccHistogram The pointer which stores the SRAM ECC histogram of the device.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetSramEccHistogram(cndevSramEccHistogram_t *eccHistogram, cndevDevice_t device);

// Group: ECC Management
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

// Group: ECC Management
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

// Group: ECC Management
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
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetRemappedRows(cndevRemappedRow_t *rows, cndevDevice_t device);

// Group: ECC Management
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
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetRemappedRowsV2(cndevRemappedRowV2_t *rows, cndevDevice_t device);

// Group: ECC Management
/**
 * @brief Gets the repair status.
 *
 * Gets the status of memory error repair mechanism. @p isRetirePending in @p status indicates whether or
 * not there are pending page retire. Cancel the application in which memory error is
 * occurred. @p isPending in @p status indicates whether or not there are pending hardware repairing.
 * Driver reload will be required to actually trigger hardware repair. @p isFailure in @p status
 * will be set if hardware repair ever failed in the past. A pending hardware repair won't affect
 * ongoing operations on the MLU because error-containment and dynamic page retirement mechanism
 * will handle it.
 *
 * @param[in,out] status The pointer to the structure storing the runtime address swapping status.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetRepairStatus(cndevRepairStatus_t *status, cndevDevice_t device);

// Group: ECC Management
/**
 * @brief Gets the address swapping information.
 *
 * @param[in,out] swaps The pointer to the structure storing the address swapping information.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetAddressSwaps(cndevAddressSwap_t *swaps, cndevDevice_t device);

// Group: ECC Management
/**
 * @brief Gets the address swapping information.
 *
 * @param[out] swaps The pointer to the structure storing the address swapping information.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetAddressSwapsV2(cndevAddressSwapV2_t *swaps, cndevDevice_t device);

// Group: ECC Management
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
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetRetiredPagesOperation(cndevRetiredPageOperation_t *operation, cndevDevice_t device);

/******************************************************************************
 * Utilization Management
 ******************************************************************************/
// Group: Utilization Management
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
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetDeviceUtilizationInfo(cndevUtilizationInfo_t *utilInfo, cndevDevice_t device);

// Group: Utilization Management
/**
 * @brief Gets the JPU codec utilization.
 *
 * @param[in,out] jpuutil The pointer to the structure storing the JPU codec utilization.
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
cndevRet_t cndevGetImageCodecUtilization(cndevImageCodecUtilization_t *jpuutil, cndevDevice_t device);

// Group: Utilization Management
/**
 * @brief Gets the VPU codec utilization.
 *
 * @param[in,out] vpuutil The pointer to the structure storing the VPU codec utilization.
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
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetVideoCodecUtilization(cndevVideoCodecUtilization_t *vpuutil, cndevDevice_t device);

// Group: Utilization Management
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
cndevRet_t cndevGetScalerUtilization(cndevScalerUtilization_t *scalerutil, cndevDevice_t device);

// Group: Utilization Management
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
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetDeviceCPUUtilizationV2(cndevDeviceCPUUtilizationV2_t *util, cndevDevice_t device);

// Group: Utilization Management
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
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetDeviceCPUSamplingInterval(cndevDeviceCPUSamplingInterval_t *time, cndevDevice_t device);

// Group: Utilization Management
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
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevSetDeviceCPUSamplingInterval(cndevDeviceCPUSamplingInterval_t *time, cndevDevice_t device);

// Group: Utilization Management
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
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetTinyCoreUtilization(cndevTinyCoreUtilization_t *util, cndevDevice_t device);

// Group: Utilization Management
/**
 * @brief Gets the information of the device's processes utilization.
 *
 * @param[in,out] processCount The size of the space which the user allocates for storing the information of processes. At the same time, it
 * is also a pointer to the size of a space that is used to store all information after the function ends.
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
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetProcessUtilization(__uint32_t *processCount, cndevProcessUtilization_t *processUtil, cndevDevice_t device);

// Group: Utilization Management
/**
 * @brief Gets the average utilization information of the device's processes between two calls.
 *
 * @param[in,out] processCount Pointer to the caller-supplied array size, and returns the count of running processes.
 * @param[out] processUtil Pointer to the caller-supplied buffer in which the average utilization information of
 * the device's processes are returned.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_INSUFFICIENT_SPACE,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetProcessUtilizationInfo(unsigned int *processCount, cndevProcessUtilizationInfo_t *processUtil, cndevDevice_t device);

/******************************************************************************
 * Power Management
 ******************************************************************************/
// Group: Power Management
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

// Group: Power Management
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
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetPowerManagementLimitation(cndevPowerManagementLimitation_t *limitation, cndevDevice_t device);

// Group: Power Management
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
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetPowerManagementDefaultLimitation(cndevPowerManagementLimitation_t *defaultLimitation, cndevDevice_t device);

// Group: Power Management
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
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetPowerManagementLimitationRange(cndevPowerManagementLimitationRange_t *limit, cndevDevice_t device);

// Group: Power Management
/**
 * @brief Gets the information of the device's power consumption. @p maxPower in @p powerInfo greater than 0 is valid.
 *
 * @param[out] powerInfo The pointer which stores the information of the device's power consumption after the
 * function ends.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetDevicePowerInfo(cndevDevicePowerInfo_t *powerInfo, cndevDevice_t device);

/******************************************************************************
 * MIM Management
 ******************************************************************************/
// Group: MIM Management
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

// Group: MIM Management
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

// Group: MIM Management
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
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetSRIOVMode(cndevSRIOVMode_t *mode, cndevDevice_t device);

// Group: MIM Management
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
 * - ::CNDEV_ERROR_IN_USE,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevSetSRIOVMode(cndevSRIOVMode_t *mode, cndevDevice_t device);

// Group: MIM Management
/**
 * @brief Creates an MLU instance by profile ID based on MIM.
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
 * - ::CNDEV_ERROR_DUPLICATE,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevCreateMluInstanceByProfileId(cndevMluInstance_t *miHandle, __uint32_t profileId, cndevDevice_t device, char *instanceName);

// Group: MIM Management
/**
 * @brief Creates an MLU instance by profile name based on MIM.
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
 * - ::CNDEV_ERROR_DUPLICATE,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevCreateMluInstanceByProfileName(cndevMluInstance_t *miHandle, char *profileName, cndevDevice_t device, char *instanceName);

// Group: MIM Management
/**
 * @brief Creates MLU instance with the specified placement by profile ID based on MIM.
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
 * - ::CNDEV_ERROR_DUPLICATE,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevCreateMluInstanceByProfileIdWithPlacement(cndevMluInstancePlacement_t *placement,
					cndevMluInstance_t *miHandle, __uint32_t profileId, cndevDevice_t device, char *instanceName);

// Group: MIM Management
/**
 * @brief Creates an MLU instance with the specified placement by profilename based on MIM.
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
 * - ::CNDEV_ERROR_DUPLICATE,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevCreateMluInstanceByProfileNameWithPlacement(cndevMluInstancePlacement_t *placement,
					cndevMluInstance_t *miHandle, char *profileName, cndevDevice_t device, char *instanceName);

// Group: MIM Management
/**
 * @brief Destroys an MLU instance by MLU instance handle based on MIM.
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
 * - ::CNDEV_ERROR_IN_USE,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevDestroyMluInstanceByHandle(cndevMluInstance_t miHandle);

// Group: MIM Management
/**
 * @brief Destroys an MLU instance by MLU instance name based on MIM.
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
 * - ::CNDEV_ERROR_IN_USE,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevDestroyMluInstanceByInstanceName(cndevDevice_t device, char *instanceName);

// Group: MIM Management
/**
 * @brief Gets MLU instance profile information based on MIM.
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
 * - ::CNDEV_ERROR_NOT_FOUND,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetMluInstanceProfileInfo(cndevMluInstanceProfileInfo_t *profileInfo,
					__int32_t profile, cndevDevice_t device);

// Group: MIM Management
/**
 * @brief Gets MLU instance possible placements based on MIM.
 *
 * @param[in,out] count The size of the space which the user allocates for storing the information of placements. At the same time,
 *   it is also the pointer which stores a pointer to the size of a space which is suited to store all information after the function ends.
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
 * - ::CNDEV_ERROR_NOT_FOUND,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetMluInstancePossiblePlacements(__int32_t *count, cndevMluInstancePlacement_t *placement,
					__int32_t profileId, cndevDevice_t device);

// Group: MIM Management
/**
 * @brief Gets MLU instance remaining capacity for the given profile ID based on MIM.
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
 * - ::CNDEV_ERROR_NOT_FOUND,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetMluInstanceRemainingCapacity(__int32_t *count, __int32_t profileId, cndevDevice_t device);

// Group: MIM Management
/**
 * @brief Gets MLU instance max count for the given profile ID based on MIM.
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
 * - ::CNDEV_ERROR_NOT_FOUND,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetMaxMluInstanceCount(__int32_t *count, __int32_t profileId, cndevDevice_t device);

// Group: MIM Management
/**
 * @brief Gets MLU instance information based on MIM.
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
 * - ::CNDEV_ERROR_NOT_FOUND,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetMluInstanceInfo(cndevMluInstanceInfo_t *miInfo, cndevMluInstance_t miHandle);

// Group: MIM Management
/**
 * @brief Gets all MLU instance information based on MIM
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
 * - ::CNDEV_ERROR_NO_PERMISSION,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetAllMluInstanceInfo(__int32_t *count, cndevMluInstanceInfo_t *miInfo, cndevDevice_t device);

// Group: MIM Management
/**
 * @brief Gets MLU instance handle for the given index under its parent device based on MIM.
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
 * - ::CNDEV_ERROR_NOT_FOUND,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetMluInstanceByIndex(cndevMluInstance_t *miHandle, __int32_t index, cndevDevice_t device);

// Group: MIM Management
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
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetDeviceHandleFromMluInstanceHandle(cndevDevice_t *devcieHanlde, cndevMluInstance_t miHandle);

// Group: MIM Management
/**
 * @brief Gets MLU instance for given profile ID based on MIM.
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
 * - ::CNDEV_ERROR_NO_PERMISSION,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetMluInstance(cndevMluInstance_t *miHandle,  __int32_t *count, __int32_t profileId, cndevDevice_t device);

// Group: MIM Management
/**
 * @brief Gets the MLU instance ID for the given MLU device handle based on MIM.
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
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetMluInstanceId(__int32_t *instanceId, cndevMluInstance_t miHandle);

// Group: MIM Management
/**
 * @brief Gets the MLU instance for the given MLU instance ID based on MIM.
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
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetMluInstanceById(cndevMluInstance_t *miHandle, __int32_t instanceId, cndevDevice_t device);

/******************************************************************************
 * sMLU Management
 ******************************************************************************/
// Group: sMLU Management
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

// Group: sMLU Management
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

// Group: sMLU Management
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
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetSMluProfileIdInfo(cndevSMluProfileIdInfo_t *profileId, cndevDevice_t device);

// Group: sMLU Management
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
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetSMluProfileInfo(cndevSMluProfileInfo_t *profileInfo, __int32_t profile, cndevDevice_t device);

// Group: sMLU Management
/**
 * @brief Creates an MLU instance by profile ID based on sMLU.
 *
 * @param[out] miHandle The MLU instance handle.
 * @param[in] profileId The MLU instance profile ID. For more information, see ::cndevGetSMluProfileIdInfo.
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
 * - ::CNDEV_ERROR_DUPLICATE,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevCreateSMluInstanceByProfileId(cndevMluInstance_t *miHandle, __uint32_t profileId, cndevDevice_t device, char *instanceName);

// Group: sMLU Management
/**
 * @brief Creates an MLU instance by profile name based on sMLU.
 *
 * @param[out] miHandle The MLU instance handle.
 * @param[in] profileName The MLU instance profile Name. For more information, see ::cndevGetSMluProfileIdInfo.
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
 * - ::CNDEV_ERROR_DUPLICATE,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevCreateSMluInstanceByProfileName(cndevMluInstance_t *miHandle, char *profileName, cndevDevice_t device, char *instanceName);

// Group: sMLU Management
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
 * - ::CNDEV_ERROR_IN_USE,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevDestroySMluInstanceByHandle(cndevMluInstance_t miHandle);

// Group: sMLU Management
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
 * - ::CNDEV_ERROR_IN_USE,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevDestroySMluInstanceByInstanceName(cndevDevice_t device, char *instanceName);

// Group: sMLU Management
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
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetSMluInstanceInfo(cndevSMluInfo_t *miInfo, cndevMluInstance_t miHandle);

// Group: sMLU Management
/**
 * @brief Updates the quota of MLU instance by MLU instance Handle based on sMLU.
 *
 * @param[in] miHandle The MLU instance handle.
 * @param[in] instance_quota The pointer which stores the information of an MLU instance.
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
 * - ::CNDEV_ERROR_NOT_FOUND,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevUpdateSMluInstanceQuotaByHandle(cndevMluInstance_t miHandle, cndevSMluSet_t *instance_quota);

// Group: sMLU Management
/**
 * @brief Updates the quota of an MLU instance by MLU instance name based on sMLU.
 *
 * @param[in] device The identifier of the target device.
 * @param[in] instanceName The MLU instance name.
 * @param[in] instance_quota The pointer which stores the information of an MLU instance.
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
 * - ::CNDEV_ERROR_NOT_FOUND,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevUpdateSMluInstanceQuotaByInstanceName(cndevDevice_t device, char *instanceName, cndevSMluSet_t *instance_quota);

// Group: sMLU Management
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
 * - ::CNDEV_ERROR_INSUFFICIENT_SPACE,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevGetAllSMluInstanceInfo(__int32_t *count, cndevSMluInfo_t *miInfo, cndevDevice_t device);

// Group: sMLU Management
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
 * - ::CNDEV_ERROR_NO_PERMISSION,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevCreateSMluProfileInfo(cndevSMluSet_t *profileInfo, __int32_t *profileId, cndevDevice_t device);

// Group: sMLU Management
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
 * - ::CNDEV_ERROR_DUPLICATE,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevDestroySMluProfileInfo(__int32_t profileId, cndevDevice_t device);

/******************************************************************************
 * Device Configs Management
 ******************************************************************************/
// Group: Device Configs Management
/**
 * @brief Sets device configurations and it will take effective after calling ::cndevDeviceActiveConfigs().
 *
 * @param[in] value The value which will be set to MLU Driver.
 * @param[in] type The type of device configuration.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_NO_PERMISSION,
 * - ::CNDEV_ERROR_IN_PROBLEM
 *
 * @note
 * - If this API fails, it is recommended to call ::cndevDeviceResetConfigs() to reset
 * - the configuration and prevent the next call to this API from failing.
 */
CNDEV_EXPORT
cndevRet_t cndevDeviceSetConfigs(unsigned long long value, cndevDeviceConfigsTypeEnum_t type, cndevDevice_t device);

// Group: Device Configs Management
/**
 * @brief Queries configuration value that has been activated in MLU Driver.
 *
 * @param[out] value The pointer which stores device configuration value.
 * @param[in] type The type of device configuration.
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_NO_PERMISSION,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevDeviceGetConfigs(unsigned long long *value, cndevDeviceConfigsTypeEnum_t type, cndevDevice_t device);

// Group: Device Configs Management
/**
 * @brief Activates the device configurations that have been set by ::cndevDeviceSetConfigs(). Ensure that no one is using the device.
 *
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_PROBLEM
 *
 *  * @note
 * - If this API fails, it is recommended to call ::cndevDeviceResetConfigs() to reset
 * - the configuration and prevent the next call to this API from failing.
 */
CNDEV_EXPORT
cndevRet_t cndevDeviceActiveConfigs(cndevDevice_t device);

// Group: Device Configs Management
/**
 * @brief Resets device configurations to default value. Ensure that no one is using the device.
 *
 * @param[in] device The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_NO_PERMISSION,
 * - ::CNDEV_ERROR_IN_USE,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevDeviceResetConfigs(cndevDevice_t device);

/******************************************************************************
 * Event Management
 ******************************************************************************/
// Group: Event Management
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

// Group: Event Management
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

// Group: Event Management
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

// Group: Event Management
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
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_TIMEOUT
 */
CNDEV_EXPORT
cndevRet_t cndevEventWait(cndevEventHandle handle, cndevEventData_t *eventData, __int32_t timeout);

// Group: Event Management
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
cndevRet_t cndevGetSupportedEventTypes(unsigned long long *eventTypes, cndevDevice_t device);

// Group: Event Management
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

/******************************************************************************
 * MPM Management
 ******************************************************************************/
// Group: MPM Management
/**
 * @brief Indicates whether the supplied device supports MPM.
 *
 * @param[in] device      The identifier of the target device.
 * @param[out] mpmSupport The pointer which stores MPM support status.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevMpmQueryDeviceSupport(cndevDevice_t device, bool *mpmSupport);

// Group: MPM Management
/**
 * @brief Allocates a sample buffer to be used with MPM.
 *
 * @param[out] mpmSample The pointer which stores the allocated sample buffer.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_MALLOC
 */
CNDEV_EXPORT
cndevRet_t cndevMpmSampleAlloc(cndevMpmSample_t *mpmSample);

// Group: MPM Management
/**
 * @brief Frees an allocated sample buffer that was allocated with ::cndevMpmSampleAlloc.
 *
 * @param[in] mpmSample The sample buffer to be freed.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT
 */
CNDEV_EXPORT
cndevRet_t cndevMpmSampleFree(cndevMpmSample_t mpmSample);

// Group: MPM Management
/**
 * @brief Reads a sample of MPM metrics into the provided @p mpmSample buffer.
 *
 * @param[in] mpmSample The buffer to read the sample into.
 * @param[in] device    The identifier of the target device.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_INVALID_DEVICE_ID,
 * - ::CNDEV_ERROR_UNKNOWN,
 * - ::CNDEV_ERROR_NOT_SUPPORTED,
 * - ::CNDEV_ERROR_IN_PROBLEM
 */
CNDEV_EXPORT
cndevRet_t cndevMpmSampleGet(cndevMpmSample_t mpmSample, cndevDevice_t device);

// Group: MPM Management
/**
 * @brief Calculates MPM metrics from two samples.
 *
 * @param[in,out] metricsGet The pointer which stores the metric information of given metric IDs.
 *
 * @return
 * - ::CNDEV_SUCCESS,
 * - ::CNDEV_ERROR_UNINITIALIZED,
 * - ::CNDEV_ERROR_INVALID_ARGUMENT,
 * - ::CNDEV_ERROR_UNSUPPORTED_API_VERSION
 *
 * @note
 * - If the interval between the calls of ::cndevMpmSampleGet that returns two samples is less than 100ms,
 *   the function returns ::CNDEV_SUCCESS, the status of all metrics is ::CNDEV_SUCCESS,
 *   but the values of all metrics are NaN.
 */
CNDEV_EXPORT
cndevRet_t cndevMpmMetricsGet(cndevMpmMetricsGet_t *metricsGet);

#if defined(__cplusplus)
}
#endif  /*__cplusplus*/
#endif  // INCLUDE_CNDEV_H_
