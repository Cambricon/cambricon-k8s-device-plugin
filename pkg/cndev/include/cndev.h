#ifndef __CNDEV_H
#define __CNDEV_H

#include <libgen.h>
#include <linux/pci_regs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#if defined(__cplusplus)
extern "C" {
#endif /*__cplusplus*/

// api versions
#define CNDEV_VERSION_1 1
#define CNDEV_VERSION_2 2
#define CNDEV_VERSION_3 3
#define CNDEV_VERSION_4 4
#define CNDEV_VERSION_5 5

#define EXPORT __attribute__((visibility("default")))

/**< Error codes */
typedef enum {
  CNDEV_SUCCESS = 0,                       /**< No error */
  CNDEV_ERROR_NO_DRIVER = 1,               /**< No Driver */
  CNDEV_ERROR_LOW_DRIVER_VERSION = 2,      /**< Driver Version Low*/
  CNDEV_ERROR_UNSUPPORTED_API_VERSION = 3, /**< API Version is not support*/
  CNDEV_ERROR_UNINITIALIZED = 4,           /**< API not Initial*/
  CNDEV_ERROR_INVALID_ARGUMENT = 5,        /**< Invalid pointer*/
  CNDEV_ERROR_INVALID_DEVICE_ID = 6,       /**< Invalid device id*/
  CNDEV_ERROR_UNKNOWN = 7,                 /**< Unknown error*/
  CNDEV_ERROR_MALLOC = 8,                  /**< internal malloc fail*/
  CNDEV_ERROR_INSUFFICIENT_SPACE = 9,      /**< cndevInfoCount has not enough space*/
  CNDEV_ERROR_NOT_SUPPORTED = 10           /**< not supported*/
} cndevRet_t;

typedef enum {
  MLU100 = 0, /**< MLU100 */
  MLU270 = 1, /**< MLU270 */
} cndevNameEnum_t;

typedef enum {
  SELF = 0,
  INTERNAL = 1,    /**< devices that are on the same board*/
  SINGLE = 2,      /**< all devices that only need traverse a single PCIe switch*/
  MULTIPLE = 3,    /**< all devices that need not traverse a host bridge*/
  HOST_BRIDGE = 4, /**< all devices that are connected to the same host bridge*/
  CPU = 5,         /**< all devices that are connected to the same CPU but possibly multiple host bridges*/
  SYSTEM = 6       /**<all device in the system*/
} cndevTopologyRelationshipEnum_t;
/**
 * @ brief translate the error ID to the corresponding message
 *
 * @ param errorId the error ID
 *
 *@ return "Cndev_ERROR not found!" if the corresponding message cant't be found, otherwise the corresponding message will be
 *returned
 */
EXPORT
const char *cndevGetErrorString(cndevRet_t errorId);

#ifndef cndevCheckErrors
#define __cndevCheckErrors(err, file, line)                                                                                    \
  do {                                                                                                                         \
    cndevRet_t _err = (err);                                                                                                   \
    if (CNDEV_SUCCESS != _err) {                                                                                               \
      fprintf(stderr, "cndevCheckErrors(%d): %s, from file <%s>, line %i.\n", _err, cndevGetErrorString(_err), basename((char*)file), \
              line);                                                                                                           \
      exit(1);                                                                                                                 \
    }                                                                                                                          \
  } while (0)

#define cndevCheckErrors(err) __cndevCheckErrors((err), __FILE__, __LINE__)
#endif

/**< Card information */
typedef struct {
  int version;     /**< driver version*/
  unsigned Number; /**< card_id*/
} cndevCardInfo_t;

/**< Memory information */
typedef struct {
  int version;
  __int64_t PhysicalMemoryTotal; /**< MLU physical total memory, unit:MB*/
  __int64_t PhysicalMemoryUsed;  /**< MLU physical used memory, unit:MB*/
  __int64_t VirtualMemoryTotal;  /**< MLU virtual total memory, unit:MB*/
  __int64_t VirtualMemoryUsed;   /**< MLU virtual used memory, unit:MB*/
} cndevMemoryInfo_t;

/**< Version information */
typedef struct {
  int version;
  unsigned MCUMajorVersion;    /**< MCU major id*/
  unsigned MCUMinorVersion;    /**< MCU minor id*/
  unsigned MCUBuildVersion;    /**< MCU build id*/
  unsigned DriverMajorVersion; /**< Driver major id*/
  unsigned DriverMinorVersion; /**< Driver minor id*/
  unsigned DriverBuildVersion; /**< Driver build id*/
} cndevVersionInfo_t;

/**< ECC information */
typedef struct {
  int version;
  __uint64_t OneBitError;           /**< single single-bit error*/
  __uint64_t MultipleOneError;      /**< multiple single-bit error*/
  __uint64_t MultipleError;         /**< single multiple-bits error*/
  __uint64_t MultipleMultipleError; /**< multiple multiple-bits error*/
  __uint64_t CorrectedError;        /**< corrected error*/
  __uint64_t UncorrectedError;      /**< uncorrected error*/
  __uint64_t TotalError;            /**< ECC error total times*/
} cndevECCInfo_t;

/**< Power information */
typedef struct {
  int version; /**< API version*/
  int Usage;   /**< current power dissipation,unit:W*/
  int Cap;     /**< cap power dissipation unit:W*/
} cndevPowerInfo_t;

/**< Temperature information */
typedef struct {
  int version;    /**< API version*/
  int Board;      /**< MLU board temperature, unit:℃ */
  int Cluster[8]; /**< MLU cluster temperature, unit:℃ */
} cndevTemperatureInfo_t;

/**< Fan speed information*/
typedef struct {
  int version;  /**< API version*/
  int FanSpeed; /**< MLU fan speed，the percentage of the max fan speed*/
} cndevFanSpeedInfo_t;

/**< LLC information*/
typedef struct {
  int version;      /**< API version*/
  __uint64_t Total; /**< LLC total times*/
  __uint64_t Hit;   /**< LLC hit times*/
} cndevLLCInfo_t;

/**< MLU utilization information*/
typedef struct {
  int version;             /**< API version*/
  int BoardUtilization;    /**< MLU board utilization*/
  int CoreUtilization[32]; /**< MLU core utilization*/
} cndevUtilizationInfo_t;

/**< MLU frequency information*/
typedef struct {
  int version;   /**< API version*/
  int Frequency; /**< MLU board frequency, unit:MHz*/
  int DdrFreq;  /**< MLU ddr frequency, unit:MHz*/
} cndevFrequencyInfo_t;

/**< Process information*/
typedef struct {
  int version;                      /**< API version*/
  unsigned int pid;                 /**< pid*/
  unsigned long PhysicalMemoryUsed; /**< MLU physical memory used, unit:KiB*/
  unsigned long VirtualMemoryUsed;  /**< MLU virtual memory used, unit:KiB*/
} cndevProcessInfo_t;

/**< Library version information*/
typedef struct {
  int version;              /**< API version*/
  unsigned LibMajorVersion; /**< library major version*/
  unsigned LibMinorVersion; /**< library minor version*/
  unsigned LibBuildVersion; /**< library build version*/
} cndevLibVersionInfo_t;

/**< Card core count*/
typedef struct {
  int version; /** API version*/
  int count;   /** card core count*/
} cndevCardCoreCount_t;

/**< Card cluster count*/
typedef struct {
  int version; /** API version*/
  int count;   /** card cluster count*/
} cndevCardClusterCount_t;

/**< Card name*/
typedef struct {
  int version;        /**API version*/
  cndevNameEnum_t id; /** card name*/
} cndevCardName_t;

/**< Codec count*/
typedef struct {
  int version;
  int count; /** card codec count*/
} cndevCodecCount_t;

/**< Codec utilization*/
typedef struct {
  int version;
  int totalUtilization[20];
} cndevCodecUtilization_t;

/**< SN*/
typedef struct {
  int version;
  __int64_t sn; /** card SN in hex*/
} cndevCardSN_t;

/**device id information**/
typedef struct {
  int version;
  unsigned int subsystem_id;        /**PCIe Sub-System ID**/
  unsigned int device_id;           /**PCIe Device ID**/
  unsigned short vendor;            // NOLINT            /**PCIe Vendor ID**/
  unsigned short subsystem_vendor;  // NOLINT  /**PCIe Sub-Vendor ID**/
  unsigned int domain;              /**PCIe domain**/
  unsigned int bus;                 /**PCIe bus_num**/
  unsigned int device;              /**PCIe device, slot**/
  unsigned int function;            /**PCIe function, func**/
  const char * phy_slot;                  /**Physical Slot**/
} cndevPCIeInfo_t;

/**PCie throughput,read and write**/
typedef struct {
  int version;          /** API version*/
  __int64_t pcie_read;  /** PCIe throughput read ,unit: Byte*/
  __int64_t pcie_write; /** PCIe throughput write ,unit: Byte*/
} cndevPCIethroughput_t;

/**device affinity information**/
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
  int version;      /**API version**/
  int currentSpeed; /**PCI current speed**/
  int currentWidth; /**PCI current width**/
} cndevCurrentPCIInfo_t;

typedef struct cndevTopologyNodeCapInfo_t {
  struct cndevTopologyNodeCapInfo_t *next;
  __uint8_t id;
  __uint16_t cap;
} cndevTopologyNodeCapInfo_t;

typedef struct cndevTopologyNode_t {
  int virtual_root_node;  // bool
  int domain;
  int bus;
  int device;
  int function;
  unsigned int subsystem_id;
  unsigned int device_id;
  unsigned int vendor;
  unsigned int subsystem_vendor;
  char const *device_name;
  unsigned int class_val;  // hex
  char const *class_name;
  struct cndevTopologyNodeCapInfo_t *first_cap;
  struct cndevTopologyNode_t *parent;
  struct cndevTopologyNode_t *left;
  struct cndevTopologyNode_t *right;
  struct cndevTopologyNode_t *child;  // first child
  unsigned int link_speed;
} cndevTopologyNode_t;

typedef struct {
  int version;
  __uint8_t id;
  __uint16_t cap;
} cndevCapabilityInfo_t;

/**< health state**/
typedef struct {
    int version;
    int health;
}cndevCardHealthState_t;

typedef struct {
    int version;
    int link_speed;
}cndevLinkSpeed_t;

typedef int (*CNDEV_TRAVERSE_CALLBACK)(cndevTopologyNode_t *current, void *userdata);
/**
 * @ brief do initialization work, check whether the API version and the MLU driver version could be supported
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_NO_DRIVER if the MLU driver is not available
 * @ return CNDEV_LOW_DRIVER if the version of the MLU driver is too low to support the cndev library
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 */
EXPORT
cndevRet_t cndevInit(int reserved);
/**
 * @ brief do aborting work
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED,if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 */
EXPORT
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
EXPORT
cndevRet_t cndevGetDeviceCount(cndevCardInfo_t *cardNum);

/**
 * @ brief get the device ID information of PCIe
 *
 * @ param devId the number of the card which the user selects, staring from 0
 * @ param deviceInfo will store a pointer to a structure which stores the information of the device Id after the function ends
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 */
EXPORT
cndevRet_t cndevGetPCIeInfo(cndevPCIeInfo_t *deviceInfo, int devId);

/**
 * @ brief get the information of card's memory
 *
 * @ param devId the number of the card which the user selects, staring from 0
 * @ param memInfo will store a pointer to a structure which stores the information of the cars's memory after the function ends
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 */
EXPORT
cndevRet_t cndevGetMemoryUsage(cndevMemoryInfo_t *memInfo, int devId);

/**
 * @ brief get the information of card's MCU version and MLU driver version
 *
 * @ param devId the number of the card which the user selects, staring from 0
 * @ param versInfo will store a pointer to a structure which stores the information of the cars' MCU version and MLU driver
 * version after the function ends
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 */
EXPORT
cndevRet_t cndevGetVersionInfo(cndevVersionInfo_t *versInfo, int devId);

/**
 * @ brief get the information of the card's ECC
 *
 * @ param devId the number of the card which the user selects, staring from 0

 * @ param eccInfo will store a pointer to a structure which stores the information of the cars' ECC
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 */
EXPORT
cndevRet_t cndevGetECCInfo(cndevECCInfo_t *eccInfo, int devId);

/**
 * @ brief get the information of card's power consumption
 *
 * @ param devId the number of the card which the user selects, staring from 0
 * @ param powerInfo will store a pointer to a structure which stores the information of the card's power consumption after the
 * function ends
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 */
EXPORT
cndevRet_t cndevGetPowerInfo(cndevPowerInfo_t *powerInfo, int devId);

/**
 * @ brief get the information of the card's frequency
 *
 * @ param devId the number of the card which the user selects, staring from 0
 * @ param freqInfo will store a pointer to a structure which stores the information of the card's frequency after the function
 * ends
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 */
EXPORT
cndevRet_t cndevGetFrequencyInfo(cndevFrequencyInfo_t *freqInfo, int devId);

/**
 * @ brief get the information of the card's temperature
 *
 * @ param devId the number of the card which the user selects, staring from 0
 * @ param tempInfo will store a pointer to a structure which stores the information of the card's temperature after the function
 * ends
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 */
EXPORT
cndevRet_t cndevGetTemperatureInfo(cndevTemperatureInfo_t *tempInfo, int devId);

/**
 * @ brief get the information of the card's LLC
 *
 * @ param devId the number of the card which the user selects, staring from 0
 * @ param llcInfo will store a pointer to a structure which stores the information of the card's LLC after the function ends
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 */
EXPORT
cndevRet_t cndevGetLLCInfo(cndevLLCInfo_t *llcInfo, int devId);

/**
 * @ brief get the information of the card's utilization
 *
 * @ param devId the number of the card which the user selects, staring from 0
 * @parm utilInfo will store a pointer to a structure which stores the information of the cars's utilization after the function
 * ends
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 */
EXPORT
cndevRet_t cndevGetDeviceUtilizationInfo(cndevUtilizationInfo_t *utilInfo, int devId);

/**
 * @ brief get the information of the card's fan speed
 *
 * @ param devId the number of the card which the user selects, staring from 0
 * @ param fanInfo will store a pointer to a structure which stores the information of the cards's fan speed after the function
 * ends
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 */
EXPORT
cndevRet_t cndevGetFanSpeedInfo(cndevFanSpeedInfo_t *fanInfo, int devId);

/**
 * @ brief get the information of the card's processes
 *
 * @ param devId the number of the card which the user selects, staring from 0
 * @ param infoCount the size of the space which the user allocates for storing the information of processes.At the same time,the
 * parameter will store a pointer to the size of a space which is suit to store all information after the function ends
 * @ param procInfo the pointer of the space which the user allocates for saving the information of processes
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_INSUFFICIENT_SPACE if the the space which the space which the user allocates is too little
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 */
EXPORT
cndevRet_t cndevGetProcessInfo(unsigned *infoCount, cndevProcessInfo_t *procInfo, int devId);

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
EXPORT
cndevRet_t cndevGetLibVersion(cndevLibVersionInfo_t *libVerInfo);

/**
 * @ brief get the count of the card's cores which the user select
 *
 * @ param devId the number of the card which the user selects, staring from 0
 * @ param cardCoreCount will store a pointer to a structure which stores the count of the cards's core after the function ends
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 */
EXPORT
cndevRet_t cndevGetCoreCount(cndevCardCoreCount_t *cardCoreCount, int devId);

/**
 * @ brief get the count of codec unit
 *
 * @ param devId the number of the card which the user selects, staring from 0
 * @ param codecCount will store a pointer to a structure which stores the count of codec after the function ends
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 */
EXPORT
cndevRet_t cndevGetCodecCount(cndevCodecCount_t *codecCount, int devId);

/**
 * @ brief get the utilization of codec unit
 *
 * @ param devId the number of the card which the user selects, staring from 0
 * @ param codecUtilization will store a pointer to a structure which stores the utilization of codec after the function ends
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 */
EXPORT
cndevRet_t cndevGetCodecUtilization(cndevCodecUtilization_t *codecUtilization, int devId);

/**
 * @ brief get the count of the card's clusters which the user select
 *
 * @ param devId the number of the card which the user selects, staring from 0
 * @ param clusCount will store a pointer to a structure which stores the count of the card's clusters after the function ends
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 */
EXPORT
cndevRet_t cndevGetClusterCount(cndevCardClusterCount_t *clusCount, int devId);

/**
 * @ brief get the lowest MLU driver version which the cndev library supports
 * @ param versInfo will store a pointer to a structure which stores the lowest MLU driver version after the function ends
 *
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 */
EXPORT
cndevRet_t cndevGetLowestSupportDriverVersion(cndevVersionInfo_t *versInfo);

/**
 * @ brief get the index of card's name
 *
 * @ param devId the number of the card which the user selects, staring from 0
 * @ param cardName will store a pointer to a structure which stores the index of a card's name after the function ends
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 */
EXPORT
cndevRet_t cndevGetCardName(cndevCardName_t *cardName, int devId);

/**
 * @ brief translate the index of a card's name to the string of the card's name
 *
 * @ param cardName the index of a card's name
 *
 *@ return "Unknown" if the string of the card's name cant't be found, otherwise the string of the card's name will be returned
 */
EXPORT
const char *cndevGetCardNameString(cndevNameEnum_t cardName);

/**
 * @ brief get the SN(serial number) of the card
 *
 * @ param devId the number of the card which the user selects, staring from 0
 * @ param cardSN will store a pointer to a structure which stores the SN of the card after the function ends
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 */
EXPORT
cndevRet_t cndevGetCardSN(cndevCardSN_t *cardSN, int devId);
/**
 * @ brief get the PCIe throughput,read and write
 *
 * @ param devId the number of the card which the user selects, staring from 0
 * @ param pciethroughput will store PCIe read and write throughput
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 */
EXPORT
cndevRet_t cndevGetPCIethroughput(cndevPCIethroughput_t *pciethroughput, int devId);

/**
 * @ brief get device related cpu affinity
 *
 * @ param devId the number of the card which the user selects, staring from 0
 * @ param affinity will store cpu affinity info
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 */
EXPORT
cndevRet_t cndevGetDeviceAffinity(cndevAffinity_t *affinity, int devId);

/**
 * @ brief clear current thread's cpu affinity, set to defalut
 *
 * @ param devId the number of the card which the user selects, staring from 0
 * @ param version cndev_version
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 */
EXPORT
cndevRet_t cndevClearCurrentThreadAffinity(int version, int devId);

/**
 * @ brief set current thread's cpu affinity to device related cpus
 *
 * @ param devId the number of the card which the user selects, staring from 0
 * @ param version cndev_version
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 */
EXPORT
cndevRet_t cndevSetCurrentThreadAffinity(int version, int devId);

/**
 * @ brief get two devices's relationship
 *
 * @ param devId1 the number of the card1, staring from 0
 * @ param devId2 the number of the card2, staring from 0
 * @ param relationship will store two devices's relationship
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 */
EXPORT
cndevRet_t cndevTopologyGetRelationship(cndevTopologyRelationship_t *relationship, int devId1, int devId2);

/**
 * @ brief get devid nearest devices by relationship
 *
 * @ param devId the number of the card, staring from 0
 * @ param version cndev_version
 * @ param count devIdArray's size
 * @ param devIdArray will store related devices's id
 * @ param rel specified relationship
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INSUFFICIENT_SPACE if the the space which the space which the user allocates is too little
 */
EXPORT
cndevRet_t cndevTopologyGetNearestDevices(int version, cndevTopologyRelationshipEnum_t rel, __uint64_t *count,
                                          __uint64_t *devIdArray, int devId);

/**
 * @ brief get two devices's relationship
 *
 * @ param cpuId specified cpu id
 * @ param version cndev_version
 * @ param count devIdArray's size
 * @ param devIdArray will store related devices's id
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INSUFFICIENT_SPACE if the the space which the space which the user allocates is too little
 */
EXPORT
cndevRet_t cndevTopologyGetCpuRelatedDevices(int version, int cpuId, __uint64_t *count, __uint64_t *devidArray);

/**
 * @ brief get the current information of PCI
 *
 * @ param devId the number of the card which the user selects, staring from 0
 * @ param currentPCIInfo will stores a pointer to a structure which stores the current information of PCI
     after the function ends
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
*/
EXPORT
cndevRet_t cndevGetCurrentPCIInfo(cndevCurrentPCIInfo_t *currentPCIInfo, int devId);

/**
 * @ brief get two nodes's relationship
 *
 * @ param node1 the topology node
 * @ param node2 the topology node
 * @ param relationship will store two devices's relationship
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 */
EXPORT
cndevRet_t cndevTopologyGetRelationshipByNode(cndevTopologyRelationship_t *relationship, cndevTopologyNode_t *node1,
                                              cndevTopologyNode_t *node2);

/**
 * @ brief get a topology tree node by bdf
 *
 * @ param version cndev version
 * @ param treeNode a target topolog tree node
 * @ param domain  the domain number of pci device
 * @ param bus the bus bus number of pci device
 * @ param device the slot number of pci device
 * @ param function the function number of pci device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 */
EXPORT
cndevRet_t cndevGetNodeByBDF(int version, cndevTopologyNode_t **treeNode, unsigned int domain, unsigned int bus,
                             unsigned int device, unsigned int function);

/**
 * @ brief get the device id by bdf
 *
 * @ param version cndev version
 * @ param devId a target device id
 * @ param domain  the domain number of pci device
 * @ param bus the bus bus number of pci device
 * @ param device the slot number of pci device
 * @ param function the function number of pci device
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 */
EXPORT
cndevRet_t cndevGetDevIdByBDF(int version, int *devId, unsigned int domain, unsigned int bus, unsigned int device,
                              unsigned int function);

/**
 * @ brief get a topology tree node by device id
 *
 * @ param version cndev version
 * @ param treeNode a target topolog tree node
 * @ param devId  the device id
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 */
EXPORT
cndevRet_t cndevGetNodeByDevId(int version, cndevTopologyNode_t **treeNode, int devId);

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
 */
EXPORT
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
 */
EXPORT
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
 */
EXPORT
cndevRet_t cndevGetNodeCapabilityInfo(cndevCapabilityInfo_t *capability, cndevTopologyNode_t *treeNode);

/**
 * @ brief get tree nodes by device name
 *
 * @ param deviceName the name of pci device
 * @ param version cndev_version
 * @ param count devIdArray's size
 * @ param nodeArray will store related devices's node
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 * @ return CNDEV_ERROR_INSUFFICIENT_SPACE if the the space which the space which the user allocates is too little
 */
EXPORT
cndevRet_t cndevGetNodeByDeviceName(int version, int *count, cndevTopologyNode_t *nodeArray, const char *deviceName);

/**
 * @ brief get the healthy state of the card
 *
 * @ param devId the number of the card which the user selects, staring from 0
 * @ param cardHealthState will stores a pointer to a structure which stores the HP of the card after the function ends, 1 means health, 0 means sick
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 */
EXPORT
cndevRet_t cndevGetCardHealthState(cndevCardHealthState_t* cardHealthState, int devId);

/**
 * @ brief get the pcie switch link speed, need sudo
 *
 * @ param devId the number of the card which the user selects, staring from 0
 * @ param linkspeed will stores a pointer to a structure which stores the pcie switch link speed
 *
 * @ return CNDEV_SUCCESS if success
 * @ return CNDEV_ERROR_UNINITIALIZED if the user don't call the function named cndevInit beforehand
 * @ return CNDEV_ERROR_INVALID_ARGUMENT if the parameter is NULL
 * @ return CNDEV_ERROR_UNKNOWN if some fault occurs, when the function calls some system function
 * @ return CNDEV_ERROR_INVALID_DEVICE_ID if the number of card which the user selects is not available
 * @ return CNDEV_UNSUPPORTED_API_VERSION if the API version is too low to be support by the cndev library
 */
EXPORT
cndevRet_t cndevGetLowestLinkSpeed(cndevLinkSpeed_t* linkspeed, int devId);

#if defined(__cplusplus)
}
#endif  /*__cplusplus*/
#endif  // __CNDEV_H
