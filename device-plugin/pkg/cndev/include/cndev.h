#ifndef INCLUDE_CNDEV_H_
#define INCLUDE_CNDEV_H_

#ifndef WIN32
#include <libgen.h>
#include <linux/pci_regs.h>
#include <unistd.h>
#endif
#include <stdio.h>
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

#define MLULINK_PORT 8
#define TINYCOREMAXCOUNT 10
#define CNDEV_MLUINSTANCE_PROFILE_NAME_BUFFER_SIZE 100
#define CNDEV_MLUINSTANCE_DEVICE_NODE_NAME_BUFFER_SIZE 100
#define CNDEV_NAME_BUFFER_SIZE 200
#define CNDEV_MLU_CLUSTER_FREQUENCY_COUNT 20
typedef __int32_t cndevDevice_t;
typedef __int32_t cndevMluInstance_t;

#if defined(WIN32) || defined(WINDOWS)
#define CNDEV_EXPORT __declspec(dllexport)
#elif defined(LINUX) || defined(__linux) || defined(__CYGWIN__)
#define CNDEV_EXPORT __attribute__((visibility("default")))
#endif

#ifdef WIN32
typedef unsigned char __uint8_t;
typedef unsigned short __uint16_t;
typedef int __int32_t;
typedef unsigned int __uint32_t;
typedef unsigned long long __uint64_t;
typedef long __int64_t;
#endif
/**< Error codes */
typedef enum {
  CNDEV_SUCCESS = 0,                       /**< No error */
  CNDEV_ERROR_NO_DRIVER = 1,               /**< No Driver */
  CNDEV_ERROR_LOW_DRIVER_VERSION = 2,      /**< Driver Version Low */
  CNDEV_ERROR_UNSUPPORTED_API_VERSION = 3, /**< API Version is not support */
  CNDEV_ERROR_UNINITIALIZED = 4,           /**< API not Initial */
  CNDEV_ERROR_INVALID_ARGUMENT = 5,        /**< Invalid pointer */
  CNDEV_ERROR_INVALID_DEVICE_ID = 6,       /**< Invalid device id */
  CNDEV_ERROR_UNKNOWN = 7,                 /**< Unknown error */
  CNDEV_ERROR_MALLOC = 8,                  /**< Internal malloc fail */
  CNDEV_ERROR_INSUFFICIENT_SPACE = 9,      /**< cndevInfoCount has not enough space */
  CNDEV_ERROR_NOT_SUPPORTED = 10,          /**< Not supported */
  CNDEV_ERROR_INVALID_LINK = 11,           /**< Invalid link port */
  CNDEV_ERROR_NO_DEVICES = 12,             /**< No MLU devices */
  CNDEV_ERROR_NO_PERMISSION = 13,          /**< No permission */
  CNDEV_ERROR_NOT_FOUND = 14,              /**< Not found */
  CNDEV_ERROR_IN_USE = 15,                 /**< MLU is currently in use */
  CNDEV_ERROR_DUPLICATE = 16,              /**< Instance name duplicate */
} cndevRet_t;

typedef enum {
  CNDEV_DEVICE_TYPE_MLU100 = 0,
  CNDEV_DEVICE_TYPE_MLU270 = 1,
  CNDEV_DEVICE_TYPE_MLU220_M2 = 16,
  CNDEV_DEVICE_TYPE_MLU220_EDGE = 17,
  CNDEV_DEVICE_TYPE_MLU220_EVB = 18,
  CNDEV_DEVICE_TYPE_MLU220_M2i = 19,
  CNDEV_DEVICE_TYPE_MLU290 = 20,
  CNDEV_DEVICE_TYPE_MLU370 = 23,
  CNDEV_DEVICE_TYPE_MLU365 = 24,
  CNDEV_DEVICE_TYPE_CE3226 = 25,
  CNDEV_DEVICE_TYPE_MLU590 = 26,
  CNDEV_DEVICE_TYPE_MLU585 = 27,
  CNDEV_DEVICE_TYPE_1V_2201 = 29,
  CNDEV_DEVICE_TYPE_MLU580 = 30,
  CNDEV_DEVICE_TYPE_MLU570 = 31,
  CNDEV_DEVICE_TYPE_1V_2202 = 32,
  // deprecated
  MLU100 = CNDEV_DEVICE_TYPE_MLU100,
  MLU270 = CNDEV_DEVICE_TYPE_MLU270,
  MLU220_M2 = CNDEV_DEVICE_TYPE_MLU220_M2,
  MLU220_EDGE = CNDEV_DEVICE_TYPE_MLU220_EDGE,
  MLU220_EVB = CNDEV_DEVICE_TYPE_MLU220_EVB,
  MLU220_M2i = CNDEV_DEVICE_TYPE_MLU220_M2i,
  MLU290 = CNDEV_DEVICE_TYPE_MLU290,
  MLU370 = CNDEV_DEVICE_TYPE_MLU370,
  MLU365 = CNDEV_DEVICE_TYPE_MLU365,
  CE3226 = CNDEV_DEVICE_TYPE_CE3226,
  MLU590 = CNDEV_DEVICE_TYPE_MLU590,
  MLU585 = CNDEV_DEVICE_TYPE_MLU585,
  MLU580 = CNDEV_DEVICE_TYPE_MLU580,
  MLU570 = CNDEV_DEVICE_TYPE_MLU570,
} cndevNameEnum_t;

typedef enum {
  SELF = 0,
  INTERNAL = 1,    /**< devices that are on the same board */
  SINGLE = 2,      /**< all devices that only need traverse a single PCIe switch */
  MULTIPLE = 3,    /**< all devices that need not traverse a host bridge */
  HOST_BRIDGE = 4, /**< all devices that are connected to the same host bridge */
  CPU = 5,         /**< all devices that are connected to the same CPU but possibly multiple host bridges */
  SYSTEM = 6       /**< all device in the system */
} cndevTopologyRelationshipEnum_t;

typedef enum {
  SPEED_FMT_NRZ = 0,
  SPEED_FMT_PM4 = 1
} cndevMLULinkSpeedEnum_t;

typedef enum {
  MLULINK_CNTR_RD_BYTE = 0,
  MLULINK_CNTR_RD_PKG = 1,
  MLULINK_CNTR_WR_BYTE = 2,
  MLULINK_CNTR_WR_PKG = 3,
  MLULINK_ERR_RPY = 4,
  MLULINK_ERR_FTL = 5,
  MLULINK_ERR_ECC_DBE = 6,
  MLULINK_ERR_CRC24 = 7,
  MLULINK_ERR_CRC32 = 8,
  MLULINK_ERR_CORR = 9,
  MLULINK_ERR_UNCORR = 10,
  MLULINK_ERR_READ_PACKAGE = 11,
  MLULINK_ERR_WRITE_PACKAGE = 12,
  MLULINK_ERR_ILLEGAL_ACCESS_PACKAGE = 13,
  MLULINK_CNTR_CNP_PKGS = 14,
  MLULINK_CNTR_PFC_PKGS = 15,
  MLULINK_COUNTER_COUNT,
} cndevMLULinkCounterEnum_t;

typedef enum {
  CNDEV_FEATURE_DISABLED = 0,
  CNDEV_FEATURE_ENABLED = 1
} cndevEnableStatusEnum_t;

typedef enum {
	CNDEV_MLUINSTANCE_PROFILE_1_SLICE,
	CNDEV_MLUINSTANCE_PROFILE_1_SLICE_MLU_2_SLICE_VPU,
	CNDEV_MLUINSTANCE_PROFILE_2_SLICE,
	CNDEV_MLUINSTANCE_PROFILE_2_SLICE_MLU_1_SLICE_MEM,
	CNDEV_MLUINSTANCE_PROFILE_3_SLICE,
	CNDEV_MLUINSTANCE_PROFILE_4_SLICE,
	CNDEV_MLUINSTANCE_PROFILE_5_SLICE,
	CNDEV_MLUINSTANCE_PROFILE_6_SLICE,
	CNDEV_MLUINSTANCE_PROFILE_COUNT,
} cndevMLUInstanceProfileEnum_t;

typedef enum {
  CNDEV_DEVICE_CONFIGS_WATCHDOG_TIMER = 0, /**< watchdog timer in seconds, need between 0 ~ 0xffffffff */
} cndevDeviceConfigsTypeEnum_t;

/**
 * @ brief translate the error ID to the corresponding message
 *
 * @ param errorId the error ID
 *
 *@ return "Cndev_ERROR not found!" if the corresponding message can't be found, otherwise the corresponding message will be
 *returned
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

/**< Card information */
typedef struct {
  int version;     /**< driver version */
  unsigned number; /**< card_id */
} cndevCardInfo_t;

/**< UUID information */
typedef struct {
  int version;
  __uint8_t uuid[CNDEV_UUID_SIZE]; /**< uuid */
  __uint64_t ncsUUID64;      /**< ncs uuid*/
} cndevUUID_t;

/**< Memory information */
typedef struct {
  int version;
  __int64_t physicalMemoryTotal;    /**< MLU physical total memory, unit:MB */
  __int64_t physicalMemoryUsed;     /**< MLU physical used memory, unit:MB */
  __int64_t virtualMemoryTotal;     /**< MLU virtual total memory, unit:MB */
  __int64_t virtualMemoryUsed;      /**< MLU virtual used memory, unit:MB */
  __int64_t channelNumber;          /**< Memory channel number */
  __int64_t channelMemoryUsed[20];  /**< Memory used each channel, unit:MB */
  __uint64_t globalMemory;
} cndevMemoryInfo_t;

/**< Version information */
typedef struct {
  int version;
  unsigned mcuMajorVersion;    /**< MCU major id */
  unsigned mcuMinorVersion;    /**< MCU minor id */
  unsigned mcuBuildVersion;    /**< MCU build id */
  unsigned driverMajorVersion; /**< Driver major id */
  unsigned driverMinorVersion; /**< Driver minor id */
  unsigned driverBuildVersion; /**< Driver build id */
} cndevVersionInfo_t;

/**< ECC information */
typedef struct {
  int version;
  __uint64_t oneBitError;           /**< single single-bit error */
  __uint64_t multipleOneError;      /**< multiple single-bit error */
  __uint64_t multipleError;         /**< single multiple-bits error */
  __uint64_t multipleMultipleError; /**< multiple multiple-bits error */
  __uint64_t correctedError;        /**< corrected error */
  __uint64_t uncorrectedError;      /**< uncorrected error */
  __uint64_t totalError;            /**< ECC error total times */
  __uint64_t addressForbiddenError;  /**< address forbidden error */
} cndevECCInfo_t;

/**< Power information */
typedef struct {
  int version;        /**< API version */
  int usage;          /**< current power dissipation,unit:W */
  int cap;            /**< cap power dissipation unit:W */
  int usageDecimal;   /**< decimal places for current power dissipation */
  int machine;        /**< current machine power dissipation,unit:W */
  int capDecimal;     /**< decimal places for cap powewr */
  __uint16_t thermalDesignPower; /**< thermal design power,unit:W */
  __uint16_t instantaneousPowerUsage;  /**< instantaneous power dissipation,unit:W */
  __uint16_t instantaneousPowerUsageDecimal;  /**< decimal places for instantaneous power dissipation */
} cndevPowerInfo_t;

/**< Temperature information */
typedef struct {
  int version;       /**< API version */
  int board;         /**< MLU board temperature, unit:℃ */
  int cluster[20];   /**< MLU cluster temperature, unit:℃ */
  int memoryDie[8];  /**< MLU memoryDie temperature, unit:℃ */
  int chip;          /**< MLU chip temperature, unit:℃ */
  int airInlet;      /**< MLU air inlet temperature, unit:℃ */
  int airOutlet;     /**< MLU air outlet temperature, unit:℃ */
  int memory;        /**< MLU external memory temperature, unit:℃ */
  int videoInput;    /**< MLU video input temperature, unit:℃ */
  int cpu;           /**< MLU cpu temperature, unit:℃ */
  int isp;           /**< MLU image signal processor, unit:℃ */
} cndevTemperatureInfo_t;

/**< Fan speed information */
typedef struct {
  int version;      /**< API version */
  int fanSpeed;     /**< MLU fan speed,the percentage of the max fan speed */
  int chassisFanCount;  /**< chassis fan count */
  int chassisFan[28];  /**< chaassis fan speed */
} cndevFanSpeedInfo_t;

/**< MLU utilization information */
typedef struct {
  int version;                /**< API version */
  int averageCoreUtilization; /**< MLU average core utilization */
  int coreUtilization[80];    /**< MLU core utilization */
} cndevUtilizationInfo_t;

/**< MLU frequency information */
typedef struct {
  int version;               /**< API version */
  int boardFreq;             /**< MLU board frequency, unit:MHz */
  int ddrFreq;               /**< MLU ddr frequency, unit:MHz */
  __uint8_t overtempDfsFlag; /**< Over-temperature dynamic frequency */
  __uint8_t fastDfsFlag;     /**< Fast dynamic frequency */
  __uint16_t mluClusterFreqCount;       /**< MLU cluster frequency count */
  __uint16_t mluClusterFreq[CNDEV_MLU_CLUSTER_FREQUENCY_COUNT];    /**< MLU cluster frequency, unit:MHz */
  int boardDefaultFreq;      /**< MLU board defaults frequency , unit:MHz */
  int boardFreqArange[2];    /**< MLU board ajustment frequency , unit:MHz */
} cndevFrequencyInfo_t;

/**< Process information */
typedef struct {
  int version;                      /**< API version */
  unsigned int pid;                 /**< pid */
  __uint64_t physicalMemoryUsed; /**< MLU physical memory used, unit:KiB */
  __uint64_t virtualMemoryUsed;  /**< MLU virtual memory used, unit:KiB */
} cndevProcessInfo_t;

/**< Library version information */
typedef struct {
  int version;              /**< API version */
  unsigned libMajorVersion; /**< library major version */
  unsigned libMinorVersion; /**< library minor version */
  unsigned libBuildVersion; /**< library build version */
} cndevLibVersionInfo_t;

/**< Card core count */
typedef struct {
  int version; /**< API version */
  int count;   /**< card core count */
} cndevCardCoreCount_t;

/**< Card cluster count */
typedef struct {
  int version; /**< API version */
  int count;   /**< card cluster count */
} cndevCardClusterCount_t;

/**< Card name */
typedef struct {
  int version;        /**< API version */
  cndevNameEnum_t id; /**< card name */
} cndevCardName_t;

/**< SN */
typedef struct {
  int version;                /**< API version */
  __uint64_t sn;              /**< card SN in hex */
  __uint64_t motherBoardSn;   /**< motherboard SN in hex */
} cndevCardSN_t;

/**< Device Form */
typedef struct {
  int version;                /**< API version */
  __uint64_t device_form;     /**< card form */
} cndevCardForm_t;

/**< device id information */
typedef struct {
  int version;
  unsigned int subsystemId;         /**< PCIe Sub-System ID */
  unsigned int deviceId;            /**< PCIe Device ID */
  __uint16_t vendor;            /**< PCIe Vendor ID */
  __uint16_t subsystemVendor;   /**< PCIe Sub-Vendor ID */
  unsigned int domain;              /**< PCIe domain */
  unsigned int bus;                 /**< PCIe bus number */
  unsigned int device;              /**< PCIe device, slot */
  unsigned int function;            /**< PCIe function, func */
  const char * physicalSlot;        /**< Physical Slot */
  int slotID;                       /**< Slot ID */
} cndevPCIeInfo_t;

/**< PCie throughput,read and write */
typedef struct {
  int version;          /**< API version */
  __int64_t pcieRead;   /**< PCIe throughput read ,unit: Byte */
  __int64_t pcieWrite;  /**< PCIe throughput write ,unit: Byte */
} cndevPCIethroughput_t;

/**< device affinity information */
typedef struct {
  int version;
  __uint32_t cpuCount;
  /* if there are 80 cpus in the system, bitmap's format is:
   * bitmap[0]:31-16:not used, 15-0:cpu79-cpu64
   * bitmap[1]:31-0:cpu63-cpu32
   * bitmap[2]:31-0:cpu31-cpu0
   */
  __uint32_t cpuAffinityBitMap[1024];
} cndevAffinity_t;

typedef struct {
  int version;
  cndevTopologyRelationshipEnum_t relation;
} cndevTopologyRelationship_t;

typedef struct {
  int version;      /**< API version */
  int currentSpeed; /**< PCI current speed */
  int currentWidth; /**< PCI current width */
} cndevCurrentPCIInfo_t;

typedef struct cndevTopologyNodeCapInfo_t {
  struct cndevTopologyNodeCapInfo_t *next;
  __uint8_t id;
  __uint16_t cap;
} cndevTopologyNodeCapInfo_t;

typedef struct cndevTopologyNode_t {
  int virtualRootNode;    // bool
  int domain;
  int bus;
  int device;
  int function;
  unsigned int subsystemId;
  unsigned int deviceId;
  unsigned int vendor;
  unsigned int subsystemVendor;
  char const *deviceName;
  unsigned int classVal;  // hex
  char const *className;
  struct cndevTopologyNodeCapInfo_t *firstCap;
  struct cndevTopologyNode_t *parent;
  struct cndevTopologyNode_t *left;
  struct cndevTopologyNode_t *right;
  struct cndevTopologyNode_t *child;  // first child
  unsigned int linkSpeed;
  int isBridge;  // bool
  int isCardbus;  // bool
  // if is_bridge or is_cardbus, the following fields will be filled, otherwise these will be 0.
  unsigned char primaryBus;
  unsigned char secondaryBus;
  unsigned char subordinateBus;
  int acsCtrl;
} cndevTopologyNode_t;

typedef struct {
  int version;
  __uint8_t id;
  __uint16_t cap;
} cndevCapabilityInfo_t;

typedef enum {
  CNDEV_HEALTH_STATE_DEVICE_IN_PROBLEM = 0,
  CNDEV_HEALTH_STATE_DEVICE_GOOD = 1,
} cndevHealthStateEnum_t;
typedef enum {
  CNDEV_HEALTH_STATE_DRIVER_EARLY_INITED = 0,
  CNDEV_HEALTH_STATE_DRIVER_BRING_UP = 1,
  CNDEV_HEALTH_STATE_DRIVER_BOOTING = 2,
  CNDEV_HEALTH_STATE_DRIVER_LATEINIT = 3,
  CNDEV_HEALTH_STATE_DRIVER_RUNNING = 4,
  CNDEV_HEALTH_STATE_DRIVER_BOOT_ERROR = 5,
  CNDEV_HEALTH_STATE_DRIVER_RESET = 6,
  CNDEV_HEALTH_STATE_DRIVER_RESET_ERROR = 7,
  CNDEV_HEALTH_STATE_DRIVER_UNKNOWN = 8,
} cndevDriverHealthStateEnum_t;
typedef struct {
  int version;
  int health;
  cndevHealthStateEnum_t deviceState;
  cndevDriverHealthStateEnum_t driverState;
} cndevCardHealthState_t;

typedef struct {
  int version;
  __uint32_t heartbeatCount;
} cndevCardHeartbeatCount_t;

/**< link speed */
typedef struct {
    int version;
    int linkSpeed;
}cndevLinkSpeed_t;

/**< vpu utilization */
typedef struct {
    int version;
    int vpuCount;
    int vpuCodecUtilization[20];
    int encoderCount;
    int encoderUtilization[20];
    int decoderCount;
    int decoderUtilization[20];
}cndevVideoCodecUtilization_t;

/**< jpu utilization */
typedef struct {
    int version;
    int jpuCount;
    int jpuCodecUtilization[20];
}cndevImageCodecUtilization_t;

/**< fast alloc memory */
typedef struct {
  int version;
  int fastMemoryTotal;
  int fastMemoryFree;
}cndevFastAlloc_t;

/**< NUMA node id */
typedef struct {
  int version;
  __int32_t nodeId;
} cndevNUMANodeId_t;

typedef struct {
    int version;
    int scalerCount;
    int scalerUtilization[20];
}cndevScalerUtilization_t;

typedef struct {
  int version;
  int codecTurbo;
}cndevCodecTurbo_t;

typedef struct {
  int version; /**< API version */
  int count;   /**< card memorydie count */
} cndevCardMemoryDieCount_t;

typedef struct {
  int version; /**< API version */
  int qdd[8];  /**< serdes port status */
} cndevQsfpddStatus_t;

/**< MLU-Link version */
typedef struct {
  int version;
  unsigned majorVersion;
  unsigned minorVersion;
  unsigned buildVersion;
} cndevMLULinkVersion_t;

typedef enum {
  CNDEV_MLULINK_STATE_DISABLE = 0,
  CNDEV_MLULINK_STATE_ENABLE = 1,
  CNDEV_MLULINK_STATE_ONGOING = 2,
  CNDEV_MLULINK_STATE_COUNT,
} cndevMLULinkStateEnum_t;

typedef enum {
  CNDEV_MLULINK_CABLE_STATE_UNCONNECTED = 0,
  CNDEV_MLULINK_CABLE_STATE_CONNECTED = 1,
  CNDEV_MLULINK_CABLE_STATE_UNIMPLEMENTED = 2,
  CNDEV_MLULINK_CABLE_STATE_COUNT,
} cndevMLULinkCableStateEnum_t;
/**< MLU-Link status */
typedef struct {
  int version;
  cndevEnableStatusEnum_t isActive;
  cndevMLULinkStateEnum_t serdesState;
  cndevMLULinkCableStateEnum_t cableState;
} cndevMLULinkStatus_t;

/**< MLU-Link speed */
typedef struct {
  int version;
  float speedValue;
  cndevMLULinkSpeedEnum_t speedFormat;
} cndevMLULinkSpeed_t;

/**< MLU-Link capability */
typedef struct {
  int version;
  unsigned p2pTransfer;
  unsigned interlakenSerdes;
} cndevMLULinkCapability_t;

/**< MLU-Link counter */
typedef struct {
  int version;
  __uint64_t cntrReadByte;
  __uint64_t cntrReadPackage;
  __uint64_t cntrWriteByte;
  __uint64_t cntrWritePackage;
  __uint64_t errReplay;
  __uint64_t errFatal;
  __uint64_t errEccDouble;
  __uint64_t errCRC24;
  __uint64_t errCRC32;
  __uint64_t errCorrected;
  __uint64_t errUncorrected;
  __uint64_t errReadPackage;
  __uint64_t errWritePackage;
  __uint64_t errIllegalAccess;
  __uint64_t cntrCnpPackage;
  __uint64_t cntrPfcPackage;
} cndevMLULinkCounter_t;

/**< reset MLU-Link counter */
typedef struct {
  int version;
  cndevMLULinkCounterEnum_t setCounter;
} cndevMLULinkSetCounter_t;

/**< MLU-Link remote information */
typedef struct {
  int version;
  __uint64_t mcSn;
  __uint64_t baSn;
  __uint32_t slotId;
  __uint32_t portId;
  __uint8_t devIp[16];
  __uint8_t uuid[CNDEV_UUID_SIZE];
  __uint32_t devIpVersion;
  __uint32_t isIpValid;
  __int32_t connectType;
  __uint64_t ncsUUID64;
  __uint8_t  mac_addr[CNDEV_MAC_ADDRESS_LEN];
  char port_name[CNDEV_MLU_REMOTE_PORT_NAME_LEN];
} cndevMLULinkRemoteInfo_t;

/**< MLU-Link devices sn */
typedef struct {
  int version;
  __uint64_t mlulinkMcSn[8];
  __uint64_t mlulinkBaSn[8];
} cndevMLULinkDevSN_t;

typedef struct {
  __uint8_t nvmeSn[21];
  __uint8_t nvmeModel[17];
  __uint8_t nvmeFw[9];
  __uint8_t nvmeMfc[9];
} cndevNvmeSsdInfo_t;

typedef struct {
  __uint8_t psuSn[17];
  __uint8_t psuModel[17];
  __uint8_t psuFw[17];
  __uint8_t psuMfc[17];
} cndevPsuInfo_t;

typedef struct {
  __uint8_t ibSn[25];
  __uint8_t ibModel[17];
  __uint8_t ibFw[5];
  __uint8_t ibMfc[9];
} cndevIbInfo_t;

typedef struct {
  int version;
  __uint64_t chassisSn; /**< chassis sn */
  char chassisProductDate[12];
  char chassisPartNum[13];

  char chassisVendorName[17];

  __uint8_t nvmeSsdNum;
  cndevNvmeSsdInfo_t nvmeInfo[8];

  __uint8_t ibBoardNum;
  cndevIbInfo_t ibInfo[8];

  __uint8_t psuNum;
  cndevPsuInfo_t psuInfo[6];
} cndevChassisInfo_t;

typedef struct {
  int version;
  __uint16_t pcieReversion;      /**< PCIe firmware reversion */
  __uint16_t pcieBuildID;        /**< PCIe firmware build id */
  __uint16_t pcieEngineeringId;  /**< PCIe firmware engineering id */
} cndevPCIeFirmwareVersion_t;

typedef struct {
  int version;
  __uint16_t chipUtilization;
  __uint8_t coreNumber;
  __uint8_t coreUtilization[80];
} cndevDeviceCPUUtilization_t;

typedef struct {
  int version;
  __uint32_t samplingInterval;
} cndevDeviceCPUSamplingInterval_t;

typedef enum {
  CNDEV_PAGE_RETIREMENT_CAUSE_MULTIPLE_SINGLE_BIT_ECC_ERRORS = 0,
  CNDEV_PAGE_RETIREMENT_CAUSE_DOUBLE_BIT_ECC_ERROR = 1
} cndevRetirePageCauseEnum_t;

typedef struct {
  int version;
  cndevRetirePageCauseEnum_t cause;
  __uint32_t pageCount;
  __uint64_t pageAddress[512];
} cndevRetiredPageInfo_t;

typedef struct {
  int version;
  __uint32_t isPending;
  __uint32_t isFailure;
} cndevRetiredPageStatus_t;

typedef struct {
  int version;
  __uint32_t correctRows;
  __uint32_t uncorrectRows;
  __uint32_t pendingRows;
  __uint32_t failedRows;
} cndevRemappedRow_t;

typedef struct {
  int version;
  cndevEnableStatusEnum_t retirePageOption;
} cndevRetiredPageOperation_t;

typedef struct {
  int version;
  int vfState;
} cndevCardVfState_t;

typedef enum {
  PORT_WORK_MODE_UNINITIALIZED = 0,
  PORT_WORK_MODE_ALL_SUPPORT = 1,
  PORT_WORK_MODE_MLULINK = 2,
  PORT_WORK_MODE_MLULINK_OVER_ROCE = 3,
} cndevPortModeEnum_t;

typedef struct {
  int version;
  cndevPortModeEnum_t mode;
  cndevPortModeEnum_t supportMode;
} cndevMLULinkPortMode_t;

typedef enum {
  CNDEV_MLULINK_FIELD_IP_VERSION = 0,
  CNDEV_MLULINK_FIELD_VLAN_TPID,
  CNDEV_MLULINK_FIELD_VLAN_CFI,
  CNDEV_MLULINK_FIELD_VLAN_VID,
  CNDEV_MLULINK_FIELD_VLAN_EN,
  CNDEV_MLULINK_FIELD_IP_TTL,
  CNDEV_MLULINK_FIELD_FLOW_LABLE,
  CNDEV_MLULINK_FIELD_HOP_LIMIT,
  CNDEV_MLULINK_FIELD_PFC_XON_ENABLE,
  CNDEV_MLULINK_FIELD_PFC_XOFF_ENABLE,
  CNDEV_MLULINK_FIELD_PFC_XON_PERIOD,
  CNDEV_MLULINK_FIELD_PFC_XOFF_PERIOD,
  CNDEV_MLULINK_FIELD_PFC_PERIOD,
  CNDEV_MLULINK_FIELD_PFC_EN,
  CNDEV_MLULINK_FIELD_QOS_TRUST,
  CNDEV_MLULINK_FIELD_VLAN_DOT1P,
  CNDEV_MLULINK_FIELD_DATA_DOT1P,
  CNDEV_MLULINK_FIELD_CTRL_DOT1P,
  CNDEV_MLULINK_FIELD_RESP_DOT1P,
  CNDEV_MLULINK_FIELD_TCP_DSCP,
  CNDEV_MLULINK_FIELD_DATA_DSCP,
  CNDEV_MLULINK_FIELD_CTRL_DSCP,
  CNDEV_MLULINK_FIELD_RESP_DSCP,
  CNDEV_MLULINK_FIELD_NUM,
}  cndevMLULinkOverRoCEFieldEnum_t;

typedef struct {
  int version;
  cndevMLULinkOverRoCEFieldEnum_t field;
  __uint32_t value;
} cndevMLULinkOverRoCECtrl_t;

typedef struct {
  int version;
  int tinyCoreCount;
  int tinyCoreUtilization[TINYCOREMAXCOUNT];
} cndevTinyCoreUtilization_t;

typedef struct {
  int version;
  union {
    __int64_t armOsMemoryTotal;
    __int64_t deviceSystemMemoryTotal;   /**< Device system total memory, unit:KB */
  };
  union {
    __int64_t armOsMemoryUsed;
    __int64_t deviceSystemMemoryUsed;  /**< Device system used memory, unit:KB */
  };
} cndevArmOsMemoryInfo_t;

typedef struct {
  int version;
  __uint8_t chipId;
} cndevChipId_t;
typedef struct {
  int version;
  __uint8_t mluFrequencyLockStatus;
} cndevMLUFrequencyStatus_t;

typedef struct {
  int version;
  __uint8_t ipVersion;
  char ip[CNDEV_IP_ADDRESS_LEN];
} cndevMLULinkPortIP_t;

typedef struct {
  int version;
  __uint64_t die2dieCRCError;       	 /**< D2D crc error */
  __uint64_t die2dieCRCErrorOverflow;  /**< D2D crc error overflow */
} cndevCRCInfo_t;

typedef struct {
  int version;
  __uint32_t dataWidth;
  __uint32_t bandWidth;
  __uint32_t bandWidthDecimal;
} cndevDDRInfo_t;

typedef struct {
  __int32_t version;
  __uint32_t minIpuFreq;  /**< requested minimum MLU Core frequency in MHz */
  __uint32_t maxIpuFreq;  /**< requested maximum MLU Core frequency in MHz */
} cndevSetIpuFrequency_t;

typedef struct {
  __int32_t version;
  __uint32_t pid;
  __uint32_t ipuUtil;
  __uint32_t jpuUtil;
  __uint32_t vpuDecUtil;
  __uint32_t vpuEncUtil;
  __uint32_t memUtil;
} cndevProcessUtilization_t;

typedef struct {
  __int32_t version;
  __uint32_t supportedIpuFreq;  /**< supported MLU Core frequency in MHz */
} cndevSupportedIpuFrequency_t;

typedef enum {
  CNDEV_XID_NO_ERROR = 0,
  /* general user application faults */
  CNDEV_XID_SW_NOTIFY_ERROR,
  /* internal micro-controller error */
  CNDEV_XID_MCU_ERROR,
  /* DDR or HBM ECC error */
  CNDEV_XID_ECC_ERROR,
  /* commu or ipcm error */
  CNDEV_XID_RPC_ERROR,
  /* mem/resource access error */
  CNDEV_XID_ILLEGAL_ACCESS_ERROR,
  /* d2d CRC error */
  CNDEV_XID_D2D_CRC_ERROR,
  CNDEV_XID_CRC_ERROR = CNDEV_XID_D2D_CRC_ERROR,
  /* mlulink error */
  CNDEV_XID_MLULINK_ERROR,
  CNDEV_XID_MLULINK_FATAL_ERROR = CNDEV_XID_MLULINK_ERROR,
  /* hbm & ddr error */
  CNDEV_XID_HBM_ERROR,
  /* over-tempertature */
  CNDEV_XID_OVER_TEMP_ERROR,
  /* previously halt */
  CNDEV_XID_PREV_HALT_ERROR,
  /* pcie replay error */
  CNDEV_XID_PCIE_REPLAY_ERROR,
  /* heartbeat err */
  CNDEV_XID_HEARTBEAT_ERROR,
  /* ecc page retirement or row remapping */
  CNDEV_XID_PAGE_RETIREMENT_ERROR,
  /* fallen off the bus */
  CNDEV_XID_FALLEN_OFF_ERROR,
  /* dbe error */
  CNDEV_XID_DBE_ECC_ERROR,
  /* dma fault */
  CNDEV_XID_PCIE_DMA_ERROR,
  /* XPU core dump */
  CNDEV_XID_STOPPED_PROCESSING_ERROR,
  /* dev smmu error */
  CNDEV_XID_SMMU_ERROR,
  /* mlulink replay error */
  CNDEV_XID_MLULINK_REPLAY_ERROR,
  /* IPU reset error */
  CNDEV_XID_IPU_RESET_ERROR,

  CNDEV_XID_MAX_COUNT,
} cndevXidEnum_t;

typedef struct {
  __int32_t version;
  __uint32_t lastXid;       /**< last xid error */
  __uint32_t totalXidCount;   /**< total xid count */
  __uint32_t totalXid[CNDEV_XID_MAX_COUNT];  /**< total xid */
} cndevXidError_t;

typedef struct {
  __int32_t version;
  cndevEnableStatusEnum_t xidStatus[CNDEV_XID_MAX_COUNT];  /**< xid switch status */
} cndevXidStatus_t;

typedef struct {
  __int32_t version;
  __uint32_t selectAll;  /**< select all xid error switch status */
  __uint32_t selectedXid;  /**< select one xid error switch status */
  cndevEnableStatusEnum_t setXidStatus;  /**< xid error switch status */
} cndevSetXid_t;

typedef struct {
  __int32_t version;
  __uint16_t minPowerLimit;         /**< minimum power management limit */
  __uint16_t minPowerLimitDecimal;  /**< minimum power management limit, 2 decimal place */
  __uint16_t maxPowerLimit;         /**< maximum power management limit */
  __uint16_t maxPowerLimitDecimal;  /**< maximum power management limit, 2 decimal place*/
} cndevPowerManagementLimit_t;

typedef struct {
  __int32_t version;
  __uint32_t powerOffCounter;    /**< overtemperature power-off times */
  __uint32_t underClockCounter;  /**< overtemperature under clocking times */
  __int16_t powerOffTemp;        /**< overtemperature power-off tmeperature, unit:℃ */
  __int16_t underClockTemp;      /**< overtemperature under clocking tmeperature, unit:℃ */
} cndevOverTemperatureInfo_t;

typedef struct {
  __int32_t version;
  cndevEnableStatusEnum_t tdp;
  cndevEnableStatusEnum_t powerSetting;
  cndevEnableStatusEnum_t clockSetting;
  cndevEnableStatusEnum_t frequencyLocked;
  cndevEnableStatusEnum_t powerBrake;
  cndevEnableStatusEnum_t thermalSlowdown;
} cndevPerformanceThrottleReason_t;

typedef struct {
  __uint32_t version;
  __uint64_t int4Tensor;   /**< Peak INT4 Tensor MACs per cycle per core */
  __uint64_t int8Tensor;   /**< Peak INT8 Tensor MACs per cycle per core */
  __uint64_t int16Tensor;  /**< Peak INT16 Tensor MACs per cycle per core */
  __uint64_t fp16Tensor;   /**< Peak FP16 Tensor MACs per cycle per core */
  __uint64_t bf16Tensor;   /**< Peak BF16 Tensor MACs per cycle per core */
  __uint64_t fp32Tensor;   /**< Peak FP32 Tensor MACs per cycle per core */
  __uint64_t fp16Vector;   /**< Peak FP16 Vector OP per cycle per core */
  __uint64_t bf16Vector;   /**< Peak BF16 Vector OP per cycle per core */
  __uint64_t fp32Vector;   /**< Peak FP32 Vector OP per cycle per core */
  __uint64_t int8Vector;   /**< Peak INT8 Vector OP per cycle per core */
  __uint64_t int16Vector;  /**< Peak INT16 Vector OP per cycle per core */
} cndevDeviceMaxPerformance_t;

typedef enum {
  CNDEV_COMPUTEMODE_DEFAULT = 0,
  CNDEV_COMPUTEMODE_EXCLUSIVE = 1,
  CNDEV_COMPUTEMODE_COUNT,
} cndevComputeModeEnum_t;

typedef struct {
  __int32_t version;
  cndevComputeModeEnum_t mode;
} cndevComputeMode_t;

typedef struct {
  __int32_t version;
  __uint32_t counter;
} cndevPcieReplayCounter_t;

typedef struct {
  __int32_t version;
  cndevEnableStatusEnum_t sriovMode;
} cndevSRIOVMode_t;

typedef struct {
  __int32_t version;
  cndevEnableStatusEnum_t mimMode;
} cndevMimMode_t;

typedef struct {
  int version;
  int start;
  int size;
} cndevMluInstancePlacement_t;
typedef struct {
  int version;
  int profileId;
  int mluCoreCount;
  int vpuCount;
  int jpuCount;
  int gdmaCount;
  int memorySize;
  char name[CNDEV_MLUINSTANCE_PROFILE_NAME_BUFFER_SIZE];
} cndevMluInstanceProfileInfo_t;


typedef struct {
  int version;
  cndevDevice_t parentDevice;
  int instanceId;
  int profileId;
  __uint8_t uuid[CNDEV_UUID_SIZE];
  int domain;
  int bus;
  int device;
  int function;
  cndevMluInstancePlacement_t placement;
  int mluCoreCount;
  int vpuCount;
  int jpuCount;
  int gdmaCount;
  int memorySize;
  char profileName[CNDEV_MLUINSTANCE_PROFILE_NAME_BUFFER_SIZE];
  char devNodeName[CNDEV_MLUINSTANCE_DEVICE_NODE_NAME_BUFFER_SIZE];
  char ipcmDevNodeName[CNDEV_MLUINSTANCE_DEVICE_NODE_NAME_BUFFER_SIZE];
  char instanceName[CNDEV_MLUINSTANCE_DEVICE_NODE_NAME_BUFFER_SIZE];
  unsigned int reserved[128];
} cndevMluInstanceInfo_t;

typedef struct {
  int version;
  int counter;
} cndevParityError_t;

typedef struct {
  int version;
  char dockerParam[CNDEV_NAME_BUFFER_SIZE];
} cndevDockerParam_t;

typedef struct {
  int version;
  cndevEnableStatusEnum_t smluMode;
} cndevSMLUMode_t;

typedef struct {
  int version;
  __uint32_t mluQuota;
  __uint64_t memorySize;
} cndevSMluSet_t;

typedef enum {
  CNDEV_SMLU_MAX = 0,
  CNDEV_SMLU_USAGE = 1,
  CNDEV_SMLU_ITEM_COUNT,
} cndevSMluItemEnum_t;

typedef struct {
  int version;
  int profileId;
  int instanceId;
  __uint8_t uuid[CNDEV_UUID_SIZE];
  int domain;
  int bus;
  int device;
  int function;
  char profileName[CNDEV_MLUINSTANCE_PROFILE_NAME_BUFFER_SIZE];
  char instanceName[CNDEV_MLUINSTANCE_PROFILE_NAME_BUFFER_SIZE];
  char devNodeName[CNDEV_MLUINSTANCE_DEVICE_NODE_NAME_BUFFER_SIZE];
  __uint32_t mluQuota[CNDEV_SMLU_ITEM_COUNT];
  __uint64_t memorySize[CNDEV_SMLU_ITEM_COUNT];
  unsigned int reserved[128];
} cndevSMluInfo_t;

typedef struct {
  int version;
  int count;
  int profileId[128];
} cndevSMluProfileIdInfo_t;

typedef struct {
  int version;
  int profileId;
  char name[CNDEV_MLUINSTANCE_PROFILE_NAME_BUFFER_SIZE];
  __uint32_t totalCapacity;
  __uint32_t remainCapacity;
  __uint32_t mluQuota[CNDEV_SMLU_ITEM_COUNT];
  __uint64_t memorySize[CNDEV_SMLU_ITEM_COUNT];
} cndevSMluProfileInfo_t;

typedef struct {
  __uint32_t version;
  // index is cndevXidEnum_t's value
  // Indicates whether the driver supports 
  __uint64_t xidSupported[CNDEV_XID_MAX_COUNT];
  // error count
  __uint64_t xidCount[CNDEV_XID_MAX_COUNT];
} cndevXidErrorV2_t;

typedef struct {
  int version;
  __uint32_t volatileSramEccSbeCounter;
  __uint32_t volatileSramEccDbeCounter;
  __uint32_t volatileSramEccParityCounter;
  __uint32_t volatileDramEccSbeCounter;
  __uint32_t volatileDramEccDbeCounter;
  __uint64_t reserved[16];
} cndevMemEccCounter_t;

typedef int (*CNDEV_TRAVERSE_CALLBACK)(cndevTopologyNode_t *current, void *userdata);
/**
 * @ brief do initialization work, check whether the API version and the MLU driver version could be supported
 * 
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_NO_DRIVER if the MLU driver is not available
 * @ return CNDEV_LOW_DRIVER if the version of the MLU driver is too low to support the cndev library
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_NO_DEVICES if there are no MLU devices or no MLU devices can be used
 */
CNDEV_EXPORT
cndevRet_t cndevInit(int reserved);
/**
 * @ brief do aborting work
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED,if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 */
CNDEV_EXPORT
cndevRet_t cndevRelease();

/**
 * @ brief get the amount of cards
 *
 * @ param cardNum will store a pointer to a structure which stores the amount of cards after the function ends
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED,if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low (or too high) to be support by the cndev library
 */
CNDEV_EXPORT
cndevRet_t cndevGetDeviceCount(cndevCardInfo_t *cardNum);

/**
 * @ brief get the device ID information of PCIe
 *
 * @ param deviceInfo will store a pointer to a structure which stores the information of the device Id after the function ends
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 */
CNDEV_EXPORT
cndevRet_t cndevGetPCIeInfo(cndevPCIeInfo_t *deviceInfo, cndevDevice_t device);

/**
 * @ brief get the information of card's memory
 *
 * @ param memInfo will store a pointer to a structure which stores the information of the cars's memory after the function ends
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 */
CNDEV_EXPORT
cndevRet_t cndevGetMemoryUsage(cndevMemoryInfo_t *memInfo, cndevDevice_t device);

/**
 * @ brief get the information of card's MCU version and MLU driver version
 *
 * @ param versInfo will store a pointer to a structure which stores the information of the cars' MCU version and MLU driver
 * version after the function ends
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 */
CNDEV_EXPORT
cndevRet_t cndevGetVersionInfo(cndevVersionInfo_t *versInfo, cndevDevice_t device);

/**
 * @ brief get the information of the card's ECC
 *
 * @ param eccInfo will store a pointer to a structure which stores the information of the cars' ECC
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetECCInfo(cndevECCInfo_t *eccInfo, cndevDevice_t device);

/**
 * @ brief get the ecc counter of the card
 *
 * @ param eccCounter will store a pointer to a structure which stores the ecc counter of the card
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT 
cndevRet_t cndevGetMemEccCounter(cndevMemEccCounter_t *eccCounter, cndevDevice_t device);

/**
 * @ brief get the information of card's power consumption
 *
 * @ param powerInfo will store a pointer to a structure which stores the information of the card's power consumption after the
 * function ends
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetPowerInfo(cndevPowerInfo_t *powerInfo, cndevDevice_t device);

/**
 * @ brief get the information of the card's frequency
 *
 * @ param device the identifier of the target device
 * @ param freqInfo will store a pointer to a structure which stores the information of the card's frequency after the function
 * ends
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetFrequencyInfo(cndevFrequencyInfo_t *freqInfo, cndevDevice_t device);

/**
 * @ brief get the information of the card's temperature
 *
 * @ param tempInfo will store a pointer to a structure which stores the information of the card's temperature after the function
 * ends
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetTemperatureInfo(cndevTemperatureInfo_t *tempInfo, cndevDevice_t device);

/**
 * @ brief get the information of the card's utilization
 *
 * @parm utilInfo will store a pointer to a structure which stores the information of the cars's utilization after the function
 * ends
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetDeviceUtilizationInfo(cndevUtilizationInfo_t *utilInfo, cndevDevice_t device);

/**
 * @ brief get the information of the card's fan speed
 *
 * @ param fanInfo will store a pointer to a structure which stores the information of the cards's fan speed after the function
 * ends
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetFanSpeedInfo(cndevFanSpeedInfo_t *fanInfo, cndevDevice_t device);

/**
 * @ brief get the information of the card's processes
 *
 * @ param infoCount the size of the space which the user allocates for storing the information of processes.At the same time,the
 * parameter will store a pointer to the size of a space which is suit to store all information after the function ends
 * @ param procInfo the pointer of the space which the user allocates for saving the information of processes
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_INSUFFICIENT_SPACE if the space which the user allocates is too little
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 */
CNDEV_EXPORT
cndevRet_t cndevGetProcessInfo(unsigned *infoCount, cndevProcessInfo_t *procInfo, cndevDevice_t device);

/**
 *@ brief get the information of the cndev library version
 *
 * @ param libVerInfo will store a pointer to a structure which stores the information of the cndev library version after the
 *function ends
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 */
CNDEV_EXPORT
cndevRet_t cndevGetLibVersion(cndevLibVersionInfo_t *libVerInfo);

/**
 * @ brief get the count of the card's cores which the user select
 *
 * @ param cardCoreCount will store a pointer to a structure which stores the count of the cards's core after the function ends
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 */
CNDEV_EXPORT
cndevRet_t cndevGetCoreCount(cndevCardCoreCount_t *cardCoreCount, cndevDevice_t device);

/**
 * @ brief get the count of the card's clusters which the user select
 *
 * @ param clusCount will store a pointer to a structure which stores the count of the card's clusters after the function ends
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 */
CNDEV_EXPORT
cndevRet_t cndevGetClusterCount(cndevCardClusterCount_t *clusCount, cndevDevice_t device);

/**
 * @ brief get the lowest MLU driver version which the cndev library supports
 * 
 * @ param versInfo will store a pointer to a structure which stores the lowest MLU driver version after the function ends
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 */
CNDEV_EXPORT
cndevRet_t cndevGetLowestSupportDriverVersion(cndevVersionInfo_t *versInfo);

/**
 * @ brief get the index of card's name
 *
 * @ param cardName will store a pointer to a structure which stores the index of a card's name after the function ends
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 */
CNDEV_EXPORT
cndevRet_t cndevGetCardName(cndevCardName_t *cardName, cndevDevice_t device);

/**
 * @ brief translate the index of a card's name to the string of the card's name
 *
 * @ param cardName the index of a card's name
 *
 * @ return "Unknown" if the string of the card's name can't be found, otherwise the string of the card's name will be returned
 */
CNDEV_EXPORT
const char *cndevGetCardNameString(cndevNameEnum_t cardName);

/**
 * @ brief translate the index of a card's name to the string of the card's name
 *
 * @ param device the identifier of the target device
 *
 * @ return the string of the card's name by device id
 */
CNDEV_EXPORT
const char *cndevGetCardNameStringByDevId(cndevDevice_t device);

/**
 * @ brief get the SN(serial number) of the card
 *
 * @ param cardSN will store a pointer to a structure which stores the SN of the card after the function ends
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 */
CNDEV_EXPORT
cndevRet_t cndevGetCardSN(cndevCardSN_t *cardSN, cndevDevice_t device);

/**
 * @ brief get the PCIe throughput,read and write
 *
 * @ param pciethroughput will store PCIe read and write throughput
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetPCIethroughput(cndevPCIethroughput_t *pciethroughput, cndevDevice_t device);

/**
 * @ brief get device related cpu affinity
 *
 * @ param affinity will store cpu affinity info
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetDeviceAffinity(cndevAffinity_t *affinity, cndevDevice_t device);

/**
 * @ brief clear current thread's cpu affinity, set to defalut
 *
 * @ param version cndev_version
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevClearCurrentThreadAffinity(int version, cndevDevice_t device);

/**
 * @ brief set current thread's cpu affinity to device related cpus
 *
 * @ param version cndev_version
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevSetCurrentThreadAffinity(int version, cndevDevice_t device);

/**
 * @ brief get two devices's relationship
 *
 * @ param device1 the identifier of the target device1
 * @ param device2 the identifier of the target device2
 * @ param relationship will store two devices's relationship
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevTopologyGetRelationship(cndevTopologyRelationship_t *relationship, cndevDevice_t device1, cndevDevice_t device2);

/**
 * @ brief get devid nearest devices by relationship
 *
 * @ param version cndev_version
 * @ param rel specified relationship
 * @ param count deviceArray's size
 * @ param deviceArray will store related devices handle
 * @ param device the number of the card, starting from 0
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INSUFFICIENT_SPACE if the space which the user allocates is too little
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevTopologyGetNearestDevices(int version, cndevTopologyRelationshipEnum_t rel, __uint64_t *count,
                                          __uint64_t *deviceArray, cndevDevice_t device);

/**
 * @ brief get two devices's relationship
 *
 * @ param version cndev_version
 * @ param cpuId specified cpu id
 * @ param count deviceArray's size
 * @ param deviceArray will store related devices handle
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INSUFFICIENT_SPACE if the space which the user allocates is too little
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevTopologyGetCpuRelatedDevices(int version, int cpuId, __uint64_t *count, __uint64_t *deviceArray);

/**
 * @ brief get the current information of PCI
 *
 * @ param currentPCIInfo will stores a pointer to a structure which stores the current information of PCI
     after the function ends
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
*/
CNDEV_EXPORT
cndevRet_t cndevGetCurrentPCIInfo(cndevCurrentPCIInfo_t *currentPCIInfo, cndevDevice_t device);

/**
 * @ brief get two nodes's relationship
 *
 * @ param relationship will store two devices's relationship
 * @ param node1 the topology node
 * @ param node2 the topology node
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevTopologyGetRelationshipByNode(cndevTopologyRelationship_t *relationship, cndevTopologyNode_t *node1,
                                              cndevTopologyNode_t *node2);

/**
 * @ brief get a topology tree node by bdf
 *
 * @ param version cndev version
 * @ param treeNode a target topolog tree node
 * @ param domain  the domain number of pci device
 * @ param bus the bus number of pci device
 * @ param device the slot number of pci device
 * @ param function the function number of pci device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetNodeByBDF(int version, cndevTopologyNode_t **treeNode, unsigned int domain, unsigned int bus,
                             unsigned int device, unsigned int function);

/**
 * @ brief get the device id by bdf
 *
 * @ param version cndev version
 * @ param dev the identifier of the target device
 * @ param domain  the domain number of pci device
 * @ param bus the bus number of pci device
 * @ param device the slot number of pci device
 * @ param function the function number of pci device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetDevIdByBDF(int version, cndevDevice_t *dev, unsigned int domain, unsigned int bus, unsigned int device,
                              unsigned int function);

/**
 * @ brief get a topology tree node by device id
 *
 * @ param version cndev version
 * @ param treeNode a target topolog tree node
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetNodeByDevId(int version, cndevTopologyNode_t **treeNode, cndevDevice_t device);

/**
 * @ brief get the virtual root node of topology tree
 *
 * @ param version cndev version
 * @ param root the virtual root node of topology tree
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevTopologyGetVirtualRootNode(int version, cndevTopologyNode_t **root);

/**
 * @ brief traverse the topology tree with a callback function
 *
 * @ param version cndev version
 * @ param callback the name of callback function, traverse the topology tree while the return value of callback function is 1
 *         while the return value of callback function is zero, the traverse tree function break
 * @ param userdata the parameter of callback function
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevTopologyTraverseTree(int version, CNDEV_TRAVERSE_CALLBACK callback, void *userdata);

/**
 * @ brief get the capability info of tree node
 *
 * @ param capability the capability info of tree node
 * @ param treeNode a target tree node
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetNodeCapabilityInfo(cndevCapabilityInfo_t *capability, cndevTopologyNode_t *treeNode);

/**
 * @ brief get tree nodes by device name
 *
 * @ param version cndev_version
 * @ param count nodeArray's size
 * @ param nodeArray will store related devices's node
 * @ param deviceName the name of pci device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INSUFFICIENT_SPACE if the space which the user allocates is too little
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetNodeByDeviceName(int version, int *count, cndevTopologyNode_t *nodeArray, const char *deviceName);

/**
 * @ brief get the healthy state of the card
 *
 * @ param cardHealthState will stores a pointer to a structure which stores the HP of the card after the function ends, 1 means health, 0 means sick
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 */
CNDEV_EXPORT
cndevRet_t cndevGetCardHealthState(cndevCardHealthState_t* cardHealthState, cndevDevice_t device);

/**
 * @ brief get the heartbeat counter of the card
 *
 * @ param cardHeartbeatCount will stores a pointer to a structure which stores the heartbeat counter
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 */
CNDEV_EXPORT
cndevRet_t cndevGetCardHeartbeatCount(cndevCardHeartbeatCount_t* cardHeartbeatCount, cndevDevice_t device);

/**
 * @ brief get the pcie switch link speed, need sudo
 *
 * @ param linkspeed will stores a pointer to a structure which stores the pcie switch link speed
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 */
CNDEV_EXPORT
cndevRet_t cndevGetLowestLinkSpeed(cndevLinkSpeed_t* linkspeed, cndevDevice_t device);

/**
 * @ brief get the jpu codec utilization
 *
 * @ param jpu_util will stores a pointer to a structure which stores the jpu codec utilization
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetImageCodecUtilization(cndevImageCodecUtilization_t* jpuutil, cndevDevice_t device);

/**
 * @ brief get the vpu codec utilization
 *
 * @ param vpu_util will stores a pointer to a structure which stores the vpu codec utilization
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetVideoCodecUtilization(cndevVideoCodecUtilization_t* vpuutil, cndevDevice_t device);

/**
 * @ brief get the fast alloc information
 *
 * @ param fastalloc will stores a pointer to a structure which stores the fast alloc total memory and free memory
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetFastAlloc(cndevFastAlloc_t* fastalloc, cndevDevice_t device);

/**
 * @ brief get the NUMA node id of tree node
 *
 * @ param numaNodeId the NUMA node id of tree node
 * @ param treeNode a target tree node
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetNUMANodeIdByTopologyNode(cndevNUMANodeId_t* numaNodeId, cndevTopologyNode_t* treeNode);

/**
 * @ brief get the scaler utilization
 *
 * @ param scalerutil will stores a pointer to a structure which stores the scaler utilization
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetScalerUtilization(cndevScalerUtilization_t* scalerutil, cndevDevice_t device);

/**
 * @ brief get the codec turbo mode
 *
 * @ param codecTurbo will stores a pointer to a structure which stores the codec turbo information
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetCodecTurbo(cndevCodecTurbo_t* codecTurbo, cndevDevice_t device);

/**
 * @ brief get the memorydie count
 *
 * @ param memorydiecount will stores a pointer to a structure which stores the memorydie count
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetMemoryDieCount(cndevCardMemoryDieCount_t* memorydiecount, cndevDevice_t device);

/**
 * @ brief get the QSFP-DD status
 *
 * @ param qddstatus will stores a pointer to a structure which stores the QSFP-DD status
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetQsfpddStatus(cndevQsfpddStatus_t* qddstatus, cndevDevice_t device);

/**
 * @ brief get the MLU-Link version
 *
 * @ param version will stores a pointer to a structure which stores the MLU-Link version
 * @ param device the identifier of the target device
 * @ param link the number of the port which the user selects, starting from 0
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 * @ return CNDEV_ERROR_INVALID_LINK if the number of link which the user selects is not available
 */
CNDEV_EXPORT
cndevRet_t cndevGetMLULinkVersion(cndevMLULinkVersion_t *version, cndevDevice_t device, int link);
/**
 * @ brief get the MLU-Link status
 *
 * @ param status will stores a pointer to a structure which stores the MLU-Link status
 * @ param device the identifier of the target device
 * @ param link the number of the port which the user selects, starting from 0
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 * @ return CNDEV_ERROR_INVALID_LINK if the number of link which the user selects is not available
 */
CNDEV_EXPORT
cndevRet_t cndevGetMLULinkStatus(cndevMLULinkStatus_t *status, cndevDevice_t device, int link);
/**
 * @ brief get the MLU-Link speed
 *
 * @ param speed will stores a pointer to a structure which stores the MLU-Link speed
 * @ param device the identifier of the target device
 * @ param link the number of the port which the user selects, starting from 0
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 * @ return CNDEV_ERROR_INVALID_LINK if the number of link which the user selects is not available
 */
CNDEV_EXPORT
cndevRet_t cndevGetMLULinkSpeedInfo(cndevMLULinkSpeed_t *speed, cndevDevice_t device, int link);
/**
 * @ brief get the MLU-Link capability
 *
 * @ param capability will stores a pointer to a structure which stores the MLU-Link capability
 * @ param device the identifier of the target device
 * @ param link the number of the port which the user selects, starting from 0
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 * @ return CNDEV_ERROR_INVALID_LINK if the number of link which the user selects is not available
 */
CNDEV_EXPORT
cndevRet_t cndevGetMLULinkCapability(cndevMLULinkCapability_t *capability, cndevDevice_t device, int link);
/**
 * @ brief get the MLU-Link counter information
 *
 * @ param count will stores a pointer to a structure which stores the MLU-Link counter information
 * @ param device the identifier of the target device
 * @ param link the number of the port which the user selects, starting from 0
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 * @ return CNDEV_ERROR_INVALID_LINK if the number of link which the user selects is not available
 */
CNDEV_EXPORT
cndevRet_t cndevGetMLULinkCounter(cndevMLULinkCounter_t *count, cndevDevice_t device, int link);
/**
 * @ brief reset the MLU-Link counter
 *
 * @ param setcount will stores a pointer to a structure which stores the MLU-Link counter
 * @ param device the identifier of the target device
 * @ param link the number of the port which the user selects, starting from 0
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 * @ return CNDEV_ERROR_INVALID_LINK if the number of link which the user selects is not available
 * @ return CNDEV_ERROR_NO_PERMISSION the current user does not have permission for operation
 */
CNDEV_EXPORT
cndevRet_t cndevResetMLULinkCounter(cndevMLULinkSetCounter_t *setcount, cndevDevice_t device, int link);
/**
 * @ brief get the MLU-Link remote information
 *
 * @ param remoteinfo will stores a pointer to a structure which stores the MLU-Link remote information
 * @ param device the identifier of the target device
 * @ param link the number of the port which the user selects, starting from 0
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 * @ return CNDEV_ERROR_INVALID_LINK if the number of link which the user selects is not available
 */
CNDEV_EXPORT
cndevRet_t cndevGetMLULinkRemoteInfo(cndevMLULinkRemoteInfo_t *remoteinfo, cndevDevice_t device, int link);
/**
 * @ brief get the MLU-Link devices' sn
 *
 * @ param devinfo will stores a pointer to a structure which stores the MLU-Link devices sn
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetMLULinkDevSN(cndevMLULinkDevSN_t *devinfo, cndevDevice_t device);
/**
 * @ brief get the NUMA node id of tree node by device ID
 *
 * @ param numaNodeId the NUMA node id of tree node
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetNUMANodeIdByDevId(cndevNUMANodeId_t* numaNodeId, cndevDevice_t device);
/**
 * @ brief get the chassis information
 *
 * @ param chassisinfo will stores a pointer to a structure which stores the chassis information
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetChassisInfo(cndevChassisInfo_t* chassisinfo, cndevDevice_t device);
/**
 * @ brief get the PCIe firmware version information
 *
 * @ param version will stores a pointer to a structure which stores the PCIe firmware version information
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetPCIeFirmwareVersion(cndevPCIeFirmwareVersion_t *version, cndevDevice_t device);
/**
 * @ brief get the UUID information, the array of uuid don't end with '\0'
 * 
 * @ param uuidInfo will stores a pointer to a structure which stores the UUID information
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetUUID(cndevUUID_t *uuidInfo, cndevDevice_t device);
/**
 * @ brief get the device cpu utilizaion
 * 
 * @ param util will stores a pointer to a structure which stores the device cpu utilizaion
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetDeviceCPUUtilization(cndevDeviceCPUUtilization_t *util, cndevDevice_t device);
/**
 * @ brief get the device CPU refresh time
 * 
 * @ param time will stores a pointer to a structure which stores the device CPU refresh time
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetDeviceCPUSamplingInterval(cndevDeviceCPUSamplingInterval_t *time, cndevDevice_t device);
/**
 * @ brief set the device CPU refresh time
 * 
 * @ param time will stores a pointer to a structure which stores the device CPU refresh time
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevSetDeviceCPUSamplingInterval(cndevDeviceCPUSamplingInterval_t *time, cndevDevice_t device);
/**
 * @ brief return the calling thread's last-error code
 * 
 * @ return the value of the last error that occurred during the execution of this program
 */
CNDEV_EXPORT
cndevRet_t cndevGetLastError();
/**
 * @ brief get retired pages info
 *
 * @ param retirepage will stores a pointer to a structure which stores the retired pages info
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetRetiredPages(cndevRetiredPageInfo_t *retirepage, cndevDevice_t device);
/**
 * @ brief get retired pages status
 *
 * @ param retirepagestatus will stores a pointer to a structure which stores the retired pages status
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetRetiredPagesStatus(cndevRetiredPageStatus_t *retirepagestatus, cndevDevice_t device);
/**
 * @ brief get the row remapping info
 *
 * @ param time will stores a pointer to a structure which stores the device CPU refresh time
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetRemappedRows(cndevRemappedRow_t *rows, cndevDevice_t device);
/**
 * @ brief get the retired pages operation
 *
 * @ param operation will stores a pointer to a structure which stores the the retired pages operation
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetRetiredPagesOperation(cndevRetiredPageOperation_t *operation, cndevDevice_t device);

/**
 * @ brief get card VF state
 *
 * @ param vfstate will stores the state of VF
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetCardVfState(cndevCardVfState_t *vfstate, cndevDevice_t device);
/**
 * @ brief get card MLULink port mode
 *
 * @ param mode will stores the mode of card
 * @ param device the identifier of the target device
 * @ param port the number of the port which the user selects, starting from 0
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 * @ return CNDEV_ERROR_INVALID_LINK if the number of link which the user selects is not available
 */
CNDEV_EXPORT
cndevRet_t cndevGetMLULinkPortMode(cndevMLULinkPortMode_t *mode, cndevDevice_t device, int port);
/**
 * @ brief set card MLULink port mode
 *
 * @ param mode will stores the mode of card
 * @ param device the identifier of the target device
 * @ param port the number of the port which the user selects, starting from 0
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 * @ return CNDEV_ERROR_INVALID_LINK if the number of link which the user selects is not available
 */
CNDEV_EXPORT
cndevRet_t cndevSetMLULinkPortMode(cndevMLULinkPortMode_t *mode, cndevDevice_t device, int port);
/**
 * @ brief get card MLULink port roce control information
 *
 * @ param ctrl will stores roce control information
 * @ param device the identifier of the target device
 * @ param port the number of the port which the user selects, starting from 0
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 * @ return CNDEV_ERROR_INVALID_LINK if the number of link which the user selects is not available
 */
CNDEV_EXPORT
cndevRet_t cndevGetMLULinkOverRoCECtrl(cndevMLULinkOverRoCECtrl_t *ctrl, cndevDevice_t device, int port);
/**
 * @ brief get card port number
 *
 * @ param device the identifier of the target device
 *
 */
CNDEV_EXPORT
int cndevGetMLULinkPortNumber(cndevDevice_t device);

/**
 * @ brief get card tinycore utilization
 *
 * @ param util will stores the tinycore utilization
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetTinyCoreUtilization(cndevTinyCoreUtilization_t *util, cndevDevice_t device);
/**
 * @ brief get card arm os memory usage information
 *
 * @ param device the identifier of the target device
 * @ param mem will stores arm os memory usage
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetArmOsMemoryUsage(cndevArmOsMemoryInfo_t *mem, cndevDevice_t device);

/**
 * @ brief get card chip id information
 *
 * @ param chipid will stores card chip id
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetChipId(cndevChipId_t *chipid, cndevDevice_t device);
/**
 * @ brief get card MLU frequency status
 *
 * @ param status will stores  MLU frequency status
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetMLUFrequencyStatus(cndevMLUFrequencyStatus_t *status, cndevDevice_t device);
/**
 * @ brief unlock MLU frequency
 *
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 * @ return CNDEV_ERROR_NO_PERMISSION the current user does not have permission for operation
 */
CNDEV_EXPORT
cndevRet_t cndevUnlockMLUFrequency(cndevDevice_t device);
/**
 * @ brief get card MLULink port ip
 *
 * @ param ip will stores card MLULink port ip
 * @ param device the identifier of the target device
 * @ param port the number of the port which the user selects, starting from 0
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 * @ return CNDEV_ERROR_INVALID_LINK if the number of link which the user selects is not available
 */
CNDEV_EXPORT
cndevRet_t cndevGetMLULinkPortIP(cndevMLULinkPortIP_t *ip, cndevDevice_t device, int port);

/**
 * @ brief get the information of the card's D2D CRC
 *
 * @ param crcInfo will store a pointer to a structure which stores the information of the card's D2D CRC
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetCRCInfo(cndevCRCInfo_t *crcInfo, cndevDevice_t device);

/**
 * @ brief get the information of the card's DDR
 *
 * @ param ddrInfo will store a pointer to a structure which stores the information of the card's DDR
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetDDRInfo(cndevDDRInfo_t *ddrInfo, cndevDevice_t device);

/**
 * @ brief set the MLU Core frequency of the card
 *
 * @ param setipufreq will store a pointer to a structure which stores the information of the user set MLU Core frequency
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL or minIpuFreq and maxIpuFreq is not a valid frequency combination
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 * @ return CNDEV_ERROR_NO_PERMISSION the current user does not have permission for operation
 */
CNDEV_EXPORT
cndevRet_t cndevSetIpuFrequency(cndevSetIpuFrequency_t *setipufreq, cndevDevice_t device);
/**
 * @ brief get the information of the card's processes utilization
 *
 * @ param processCount the size of the space which the user allocates for storing the information of processes.At the same time,the
 * parameter will store a pointer to the size of a space which is suit to store all information after the function ends
 * @ param processUtil the pointer of the space which the user allocates for saving the information of processes utilization
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_INSUFFICIENT_SPACE if the space which the user allocates is too little
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetProcessUtilization(unsigned int *processCount, cndevProcessUtilization_t *processUtil, cndevDevice_t device);
/**
 * @ brief get the list of possible MLU Core frequency that can be used
 *
 * @ param count the size of the space which the user allocates for storing the information of supported MLU Core frequency
 * At the same time,the parameter will store a pointer to the size of a space which is suit to store all information after the function ends
 * @ param ipufreq the pointer of the space which the user allocates for saving the information of supported MLU Core frequency
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_INSUFFICIENT_SPACE if the space which the user allocates is too little
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetSupportedIpuFrequency(unsigned int *count, cndevSupportedIpuFrequency_t *ipufreq, cndevDevice_t device);

/**
 * @ brief get the information of the card's XID error
 *
 * @ param xidErr will store a pointer to a structure which stores the information of the XID error
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetXidError(cndevXidError_t *xidErr, cndevDevice_t device);

/**
 * @ brief translate the XID to the corresponding message
 *
 * @ param xid the XID
 *
 *@ return "Unknown" if the corresponding message can't be found, otherwise the corresponding message will be
 *returned
 */
CNDEV_EXPORT
const char *cndevGetXidErrorString(cndevXidEnum_t xid);

/**
 * @ brief get the information of the card's XID error switch status
 *
 * @ param xidStatus will store a pointer to a structure which stores the information of the XID error switch status
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetXidStatus(cndevXidStatus_t *xidStatus, cndevDevice_t device);

/**
 * @ brief clear the information of the card's XID error
 *
 * @ param clearXid will store a pointer to a structure which stores the information of the XID error
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 * @ return CNDEV_ERROR_NO_PERMISSION the current user does not have permission for operation
 */
CNDEV_EXPORT
cndevRet_t cndevClearXidError(cndevSetXid_t *clearXid, cndevDevice_t device);

/**
 * @ brief set the information of the card's XID error switch status
 *
 * @ param setXid will store a pointer to a structure which stores the information of the XID error
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 * @ return CNDEV_ERROR_NO_PERMISSION the current user does not have permission for operation
 */
CNDEV_EXPORT
cndevRet_t cndevSetXidStatus(cndevSetXid_t *setXid, cndevDevice_t device);

/**
 * @ brief get the information of the card's power management limit
 *
 * @ param limit will store a pointer to a structure which stores the information of the power management limit
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetPowerManagementLimit(cndevPowerManagementLimit_t *limit, cndevDevice_t device);
/**
 * @ brief get the information of the card's max performance information
 *
 * @ param maxperformance will store a pointer to a structure which stores the information of the max performance
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetDeviceMaxPerformance(cndevDeviceMaxPerformance_t *maxperformance, cndevDevice_t device);

/**
 * @ brief get the information of the card's overtemperature
 *
 * @ param tempInfo will store a pointer to a structure which stores the information of overtemperature
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetOverTemperatureInfo(cndevOverTemperatureInfo_t *tempInfo, cndevDevice_t device);

/**
 * @ brief get the information of the card's performance throttle reason
 *
 * @ param tempInfo will store a pointer to a structure which stores the information of performance throttle reason
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetPerformanceThrottleReason(cndevPerformanceThrottleReason_t *reason, cndevDevice_t device);

/**
 * @ brief get the information of the card's compute mode
 *
 * @ param mode will store a pointer to a structure which stores the information of compute mode
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetComputeMode(cndevComputeMode_t *mode, cndevDevice_t device);

/**
 * @ brief set the information of the card's compute mode
 *
 * @ param mode will store a pointer to a structure which stores the information of compute mode
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 * @ return CNDEV_ERROR_NO_PERMISSION the current user does not have permission for operation
 */
CNDEV_EXPORT
cndevRet_t cndevSetComputeMode(cndevComputeMode_t *mode, cndevDevice_t device);

/**
 * @ brief get the information of the card's PCIe replay counter
 *
 * @ param counter will store a pointer to a structure which stores the information of PCIe replay counter
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetPcieReplayCounter(cndevPcieReplayCounter_t *counter, cndevDevice_t device);

/**
 * @ brief get the information of the card's SR-IOV mode
 *
 * @ param mode will store a pointer to a structure which stores the information of SR-IOV mode
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetSRIOVMode(cndevSRIOVMode_t *mode, cndevDevice_t device);
/**
 * @ brief set the information of the card's SR-IOV mode
 *
 * @ param mode will store a pointer to a structure which stores the information of SR-IOV mode
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 * @ return CNDEV_ERROR_NO_PERMISSION the current user does not have permission for operation
 */
CNDEV_EXPORT
cndevRet_t cndevSetSRIOVMode(cndevSRIOVMode_t *mode, cndevDevice_t device);

/**
 * @ brief create MLU instance
 *
 * @ param miHandle will return the MLU instance handle
 * @ param profileId the MLU instance profile ID, refer to cndevGetMluInstanceProfileInfo
 * @ param device the identifier of the target device
 * @ param instanceName the name of the instance created
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 * @ return CNDEV_ERROR_NO_PERMISSION the current user does not have permission for operation
 * @ return CNDEV_ERROR_DUPLICATE the profile already exists
 */
CNDEV_EXPORT
cndevRet_t cndevCreateMluInstanceByProfileId(cndevMluInstance_t *miHandle, unsigned int profileId, cndevDevice_t device, char *instanceName);

/**
 * @ brief create MLU instance
 *
 * @ param miHandle will return the MLU instance handle
 * @ param profileName the MLU instance profile Name, refer to cndevGetMluInstanceProfileInfo
 * @ param device the identifier of the target device
 * @ param instanceName the name of the instance created
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL or instanceName duplicate
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 * @ return CNDEV_ERROR_NO_PERMISSION the current user does not have permission for operation
 * @ return CNDEV_ERROR_DUPLICATE the profile already exists
 */
CNDEV_EXPORT
cndevRet_t cndevCreateMluInstanceByProfileName(cndevMluInstance_t *miHandle, char *profileName, cndevDevice_t device, char *instanceName);

/**
 * @ brief create MLU instance with the specified placement
 *
 * @ param placement the requested placement
 * @ param miHandle will return the MLU instance handle
 * @ param profileId the MLU instance profile ID, refer to cndevGetMluInstanceProfileInfo
 * @ param device the identifier of the target device
 * @ param instanceName the name of the instance created
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 * @ return CNDEV_ERROR_NO_PERMISSION the current user does not have permission for operation
 * @ return CNDEV_ERROR_DUPLICATE the profile already exists
 */
CNDEV_EXPORT
cndevRet_t cndevCreateMluInstanceByProfileIdWithPlacement(cndevMluInstancePlacement_t *placement,
                                               cndevMluInstance_t *miHandle, unsigned int profileId, cndevDevice_t device, char *instanceName);

/**
 * @ brief create MLU instance with the specified placement
 *
 * @ param placement the requested placement
 * @ param miHandle will return the MLU instance handle
 * @ param profileName the MLU instance profile Name, refer to cndevGetMluInstanceProfileInfo
 * @ param device the identifier of the target device
 * @ param instanceName the name of the instance created
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL or instanceName duplicate
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 * @ return CNDEV_ERROR_NO_PERMISSION the current user does not have permission for operation
 * @ return CNDEV_ERROR_DUPLICATE the profile already exists
 */
CNDEV_EXPORT
cndevRet_t cndevCreateMluInstanceByProfileNameWithPlacement(cndevMluInstancePlacement_t *placement,
                                               cndevMluInstance_t *miHandle, char *profileName, cndevDevice_t device, char *instanceName);
/**
 * @ brief destroy MLU instance
 *
 * @ param miHandle the MLU instance handle
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 * @ return CNDEV_ERROR_NO_PERMISSION the current user does not have permission for operation
 * @ return CNDEV_ERROR_IN_USE the MLU is currently in use
 */
CNDEV_EXPORT
cndevRet_t cndevDestroyMluInstanceByHandle(cndevMluInstance_t miHandle);

/**
 * @ brief destroy MLU instance
 *
 * @ param device the identifier of the target device
 * @ param instanceName the name of the intance that want to destroy
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 * @ return CNDEV_ERROR_NO_PERMISSION the current user does not have permission for operation
 * @ return CNDEV_ERROR_IN_USE the MLU is currently in use
 */
CNDEV_EXPORT
cndevRet_t cndevDestroyMluInstanceByInstanceName(cndevDevice_t device, char *instanceName);
/**
 * @ brief get MLU instance profile information
 *
 * @ param profileInfo will store a pointer to a structure which stores the information of MLU instance profile
 * @ param profile one of the cndevMLUInstanceProfileEnum_t
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetMluInstanceProfileInfo(cndevMluInstanceProfileInfo_t * profileInfo, int profile, cndevDevice_t device);

/**
 * @ brief get MLU instance possible placements
 *
 * @ param count the size of the space which the user allocates for storing the information of placements.At the same time,
 *   the parameter will store a pointer to the size of a space which is suit to store all information after the function ends
 * @ param placement the pointer of the space which the user allocates for saving the information of placements
 * @ param profileId the MLU instance profile ID, refer to cndevGetMluInstanceProfileInfo
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetMluInstancePossiblePlacements(int *count, cndevMluInstancePlacement_t *placement, int profileId, cndevDevice_t device);

/**
 * @ brief get MLU instance remaining capacity for the given profile id
 *
 * @ param count returns remaining instance count fot the profile ID
 * @ param profileId the MLU instance profile ID, refer to cndevGetMluInstanceProfileInfo
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetMluInstanceRemainingCapacity(int *count, int profileId, cndevDevice_t device);

/**
 * @ brief get MLU instance max count for the given profile id
 *
 * @ param count returns max instance count fot the profile ID
 * @ param profileId the MLU instance profile ID, refer to cndevGetMluInstanceProfileInfo
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetMaxMluInstanceCount(int *count, int profileId, cndevDevice_t device);

/**
 * @ brief get MLU instance information
 *
 * @ param miInfo will store a pointer to a structure which stores the information of MLU instance
 * @ param miHandle the MLU instance handle
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetMluInstanceInfo(cndevMluInstanceInfo_t *miInfo, cndevMluInstance_t miHandle);

/**
 * @ brief get all MLU instance information
 *
 * @ param count the size of the space which the user allocates for storing the information of MLU instance .At the same time,
 *   the parameter will store a pointer to the size of a space which is suit to store all information after the function ends
 * @ param miInfo the pointer of the space which the user allocates for saving the information of MLU instance
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetAllMluInstanceInfo(int *count, cndevMluInstanceInfo_t *miInfo, cndevDevice_t device);

/**
 * @ brief get MLU instance handle for the given index under its parent device
 *
 * @ param miHandle the MLU instance handle
 * @ param index index of the MLU instance
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetMluInstanceByIndex(cndevMluInstance_t * miHandle, int index, cndevDevice_t device);

/**
 * @ brief get parent device handle from a MLU instance handle
 *
 * @ param devcieHanlde device handle
 * @ param miHandle the MLU instance handle
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetDeviceHandleFromMluInstanceHandle(cndevDevice_t* devcieHanlde, cndevMluInstance_t miHandle);

/**
 * @ brief get MLU instance for given profile ID
 *
 * @ param miHandle the MLU instance handle
 * @ param count the size of the space which the user allocates for storing the information of MLU instance .At the same time,
 *   the parameter will store a pointer to the size of a space which is suit to store all information after the function ends
 * @ param profileId the MLU instance profile ID, refer to cndevGetMluInstanceProfileInfo
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetMluInstance(cndevMluInstance_t* miHandle,  int *count, int profileId,  cndevDevice_t device);
/**
 * @ brief get MLU instance ID for the given MLU device handle
 *
 * @ param instanceId MLU instance ID
 * @ param miHandle the MLU instance handle
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetMluInstanceId(int * instanceId, cndevMluInstance_t miHandle);
/**
 * @ brief get MLU instance for the given MLU instance ID
 *
 * @ param miHandle the MLU instance handle
 * @ param instanceId MLU instance ID
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetMluInstanceById(cndevMluInstance_t * miHandle, int instanceId, cndevDevice_t device);
/**
 * @ brief get MLU device handle for the given index
 *
 * @ param index the index of the target MLU
 * @ param handle the MLU device handle
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_NOT_FOUND the query to find a device was not unsuccessful
 */
CNDEV_EXPORT
cndevRet_t cndevGetDeviceHandleByIndex(int index, cndevDevice_t *handle);
/**
 * @ brief get MLU device handle for the given PCI bus id
 *
 * @ param pciBusId the PCI bus id of the target MLU
 * @ param handle the MLU device handle
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_NOT_FOUND the query to find a device was not unsuccessful
 */
CNDEV_EXPORT
cndevRet_t cndevGetDeviceHandleByPciBusId(const char *pciBusId, cndevDevice_t *handle);
/**
 * @ brief get MLU device handle for the given serial number
 *
 * @ param sn the serial number of the target MLU
 * @ param handle the MLU device handle
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_NOT_FOUND the query to find a device was not unsuccessful
 */
CNDEV_EXPORT
cndevRet_t cndevGetDeviceHandleBySerial(const char *sn, cndevDevice_t *handle);
/**
 * @ brief get MLU device handle for the given UUID
 *
 * @ param uuid the UUID of the target MLU
 * @ param handle the MLU device handle
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_NOT_FOUND the query to find a device was not unsuccessful
 */
CNDEV_EXPORT
cndevRet_t cndevGetDeviceHandleByUUID(const char *uuid, cndevDevice_t *handle);
/**
 * @ brief get the parity error information
 *
 * @ param error will stores a pointer to a structure which stores the information of parity error
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetParityError(cndevParityError_t *error, cndevDevice_t handle);

/**
 * @ brief get the information of device power consumption
 *
 * @ param power will store a pointer to a structure which stores the information of the device power consumption after the
 * function ends
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetDevicePower(cndevPowerInfo_t *power, cndevDevice_t device);

/**
 * @ brief get the information of the device temperature
 *
 * @ param temp will store a pointer to a structure which stores the information of the device temperature after the function
 * ends
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetDeviceTemperature(cndevTemperatureInfo_t *temp, cndevDevice_t device);

/**
 * @ brief get the information of the card's MIM mode
 *
 * @ param mode will store a pointer to a structure which stores the information of MIM mode
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetMimMode(cndevMimMode_t *mode, cndevDevice_t device);
/**
 * @ brief set the information of the card's MIM mode
 *
 * @ param mode will store a pointer to a structure which stores the information of MIM mode
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 * @ return CNDEV_ERROR_NO_PERMISSION the current user does not have permission for operation
 */
CNDEV_EXPORT
cndevRet_t cndevSetMimMode(cndevMimMode_t *mode, cndevDevice_t device);
/**
 * @ brief get the information of the card's docker param
 *
 * @ param param will store a pointer to a structure which stores the information of docker param
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetDockerParam(cndevDockerParam_t *param, cndevDevice_t device);

/**
 * @ brief get the information of the card's SMLU mode
 *
 * @ param mode will store a pointer to a structure which stores the information of SMLU mode
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetSMLUMode(cndevSMLUMode_t *mode, cndevDevice_t device);
/**
 * @ brief set the information of the card's SMLU mode
 *
 * @ param mode will store a pointer to a structure which stores the information of SMLU mode
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 * @ return CNDEV_ERROR_NO_PERMISSION the current user does not have permission for operation
 */
CNDEV_EXPORT
cndevRet_t cndevSetSMLUMode(cndevSMLUMode_t *mode, cndevDevice_t device);
/**
 * @ brief get MLU instance profile ID count in SMLU Mode
 *
 * @ param profileId will store a pointer to a structure which stores the Id information of MLU instance profile ID
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetSMluProfileIdInfo(cndevSMluProfileIdInfo_t *profileId, cndevDevice_t device);
/**
 * @ brief get MLU instance profile information in SMLU Mode
 *
 * @ param profileInfo will store a pointer to a structure which stores the information of MLU instance profile
 * @ param profile one of the cndevMLUInstanceProfileEnum_t
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetSMluProfileInfo(cndevSMluProfileInfo_t *profileInfo, int profile, cndevDevice_t device);

/**
 * @ brief create MLU instance in SMLU Mode
 *
 * @ param miHandle will return the MLU instance handle
 * @ param profileId the MLU instance profile ID, refer to cndevGetSMluProfileIdInfo
 * @ param device the identifier of the target device
 * @ param instanceName the name of the intance that want to create
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL or profileId is invalid
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 * @ return CNDEV_ERROR_NO_PERMISSION the current user does not have permission for operation
 * @ return CNDEV_ERROR_INSUFFICIENT_SPACE the space is insufficient
 * @ return CNDEV_ERROR_DUPLICATE the profile already exists
 */
CNDEV_EXPORT
cndevRet_t cndevCreateSMluInstanceByProfileId(cndevMluInstance_t *miHandle, unsigned int profileId, cndevDevice_t device, char * instanceName);


/**
 * @ brief create MLU instance in SMLU Mode
 *
 * @ param miHandle will return the MLU instance handle
 * @ param profileName the MLU instance profile Name, refer to cndevGetSMluProfileIDInfo
 * @ param device the identifier of the target device
 * @ param instanceName the name of the intance that want to create
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL or profileName is invalid
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 * @ return CNDEV_ERROR_NO_PERMISSION the current user does not have permission for operation
 * @ return CNDEV_ERROR_INSUFFICIENT_SPACE the space is insufficient
 * @ return CNDEV_ERROR_DUPLICATE the profile already exists
 */
CNDEV_EXPORT
cndevRet_t cndevCreateSMluInstanceByProfileName(cndevMluInstance_t *miHandle, char * profileName, cndevDevice_t device, char * instanceName);

/**
 * @ brief destroy MLU instance in SMLU Mode by miHandle
 *
 * @ param miHandle the MLU instance handle
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 * @ return CNDEV_ERROR_NO_PERMISSION the current user does not have permission for operation
 * @ return CNDEV_ERROR_IN_USE the MLU is currently in use
 */
CNDEV_EXPORT
cndevRet_t cndevDestroySMluInstanceByHandle(cndevMluInstance_t miHandle);

/**
 * @ brief destroy MLU instance in SMLU Mode by instanceName
 *
 * @ param device the identifier of the target device
 * @ param instanceName the MLU instance name
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 * @ return CNDEV_ERROR_NO_PERMISSION the current user does not have permission for operation
 * @ return CNDEV_ERROR_IN_USE the MLU is currently in use
 */
CNDEV_EXPORT
cndevRet_t cndevDestroySMluInstanceByInstanceName(cndevDevice_t device, char *instanceName);

/**
 * @ brief get MLU instance information in SMLU Mode
 *
 * @ param miInfo will store a pointer to a structure which stores the information of MLU instance
 * @ param miHandle the MLU instance handle
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetSMluInstanceInfo(cndevSMluInfo_t *miInfo, cndevMluInstance_t miHandle);

/**
 * @ brief get all MLU instance information in SMLU Mode
 *
 * @ param count the size of the space which the user allocates for storing the information of MLU instance .At the same time,
 *   the parameter will store a pointer to the size of a space which is suit to store all information after the function ends
 * @ param miInfo the pointer of the space which the user allocates for saving the information of MLU instance
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 */
CNDEV_EXPORT
cndevRet_t cndevGetAllSMluInstanceInfo(int *count, cndevSMluInfo_t *miInfo, cndevDevice_t device);

/**
 * @ brief create SMLU profile
 *
 * @ param profileInfo will store a pointer to a structure which stores the information of MLU instance profile
 * @ param profileId the return value of MLU instance profile ID
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 * @ return CNDEV_ERROR_NO_PERMISSION the current user does not have permission for operation
 */
CNDEV_EXPORT
cndevRet_t cndevCreateSMluProfileInfo(cndevSMluSet_t *profileInfo, int *profileId, cndevDevice_t device);

/**
 * @ brief destroy SMLU profile
 *
 * @ param profileId the target MLU instance profile ID
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 * @ return CNDEV_ERROR_NO_PERMISSION the current user does not have permission for operation
 * @ return CNDEV_ERROR_DUPLICATE the profile already exists
 */
CNDEV_EXPORT
cndevRet_t cndevDestroySMluProfileInfo(int profileId, cndevDevice_t device);
/**
 * @ brief get the information of the card's XID error
 *
 * @ param xidErr will store a pointer to a structure which stores the information of the XID error
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 * @ return CNDEV_ERROR_NO_PERMISSION the current user does not have permission for operation
 * @ return CNDEV_ERROR_IN_USE the profile is in use, can't be deleted
 */
CNDEV_EXPORT cndevRet_t cndevGetXidErrorV2(cndevXidErrorV2_t *xidErr, cndevDevice_t device);
/**
 * @ brief inject a xid error
 *
 * @ param xidEnum the xid error to be injected
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 */
CNDEV_EXPORT cndevRet_t cndevInjectXidError(cndevXidEnum_t xidEnum, cndevDevice_t device);
/**
 * @ brief Config device configs and it will be effective after call ::cndevDeviceActiveConfigs()
 *
 * @ param value the value which will be config to driver
 * @ param type the type of device configs
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 * @ return CNDEV_ERROR_NO_PERMISSION the current user does not have permission for operation
 */
CNDEV_EXPORT
cndevRet_t cndevDeviceSetConfigs(unsigned long long value, cndevDeviceConfigsTypeEnum_t type, cndevDevice_t device);
/**
 * @ brief Query configs value that has been actived in driver
 *
 * @ param value a pointer which stores device configs value
 * @ param type the type of device configs
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 * @ return CNDEV_ERROR_NO_PERMISSION the current user does not have permission for operation
 */
CNDEV_EXPORT
cndevRet_t cndevDeviceGetConfigs(unsigned long long *value, cndevDeviceConfigsTypeEnum_t type, cndevDevice_t device);
/**
 * @ brief Active configs which has been configed by ::cndevDeviceSetConfigs(), please ensure no one is using the device
 *
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 * @ return CNDEV_ERROR_NO_PERMISSION the current user does not have permission for operation
 * @ return CNDEV_ERROR_IN_USE the MLU is currently in use
 */
CNDEV_EXPORT
cndevRet_t cndevDeviceActiveConfigs(cndevDevice_t device);
/**
 * @ brief Reset device configs to default value, please ensure no one is using the device
 *
 * @ param device the identifier of the target device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_NOT_SUPPORTED the requested operation is not available on target device
 * @ return CNDEV_ERROR_NO_PERMISSION the current user does not have permission for operation
 * @ return CNDEV_ERROR_IN_USE the MLU is currently in use
 */
CNDEV_EXPORT
cndevRet_t cndevDeviceResetConfigs(cndevDevice_t device);
#if defined(__cplusplus)
}
#endif  /*__cplusplus*/
#endif  // INCLUDE_CNDEV_H_
