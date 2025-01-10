/***********************************************************************************
 * Copyright (c) [2019-2025] by Cambricon, Inc.
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 ***********************************************************************************/

/***********************************************************************************
 *
 *  @file cntopo.h
 *
 *  @brief CNTopo (Cambricon MLU-Link Topology Tool) APIs provide
 *programmable
 *  interfaces for users to develop their-owned programs, which includes find rings
 *of
 *  MLU-Link.
 *
 ***********************************************************************************/

#ifndef CNTOPO_H_  // NOLINT
#define CNTOPO_H_  // NOLINT

#define CNTOPO_MAJOR_VERSION 1
#define CNTOPO_MINOR_VERSION 7
#define CNTOPO_PATCH_VERSION 2

#ifndef CNTOPO_VERSION
#define CNTOPO_VERSION \
  (CNTOPO_MAJOR_VERSION * 10000 + CNTOPO_MINOR_VERSION * 100 + CNTOPO_PATCH_VERSION)
#endif

#include <stdint.h>
#include <stdlib.h>

#if defined(__cplusplus)
extern "C" {
#endif
/** @brief Return values for CNTopo APIs. */
typedef enum {
  /*! Indicates no error occurred. */
  CNTOPO_RET_SUCCESS = 0x0,
  /*! Returns when CNTopo internal error occurred. */
  CNTOPO_INTERNEL_ERROR = 0x1,
  /*! Indicates CNTopo finished normally is in command line mode. */
  CNTOPO_NORMAL_FINISH = 0x10,
  /*! Indicates the value of CNTopo input is invalid. */
  CNTOPO_VALUE_ERROR = 0x12,
  /*! Indicates the file of CNTopo input is empty. */
  CNTOPO_FILE_EMPTY = 0x2,
  /*! Indicates the file path of CNTopo input is invalid. */
  CNTOPO_FILE_PATHERR = 0x21,
  /*! Returns when an error occurred during reading file content. */
  CNTOPO_FILE_ERR = 0x22,
  /*! Indicates CNTopo probe result is empty. */
  CNTOPO_PROBE_EMPTY = 0x3,
  /*! Returns when the IP address cannot be logged in. */
  CNTOPO_PARAM_ERROR = 0x31,
  /*! Indicates CNTopo ping result is empty. */
  CNTOPO_PING_EMPTY = 0x32,
  /*! Indicates CNTopo latbwtest result is empty. */
  CNTOPO_LATBWTEST_EMPTY = 0x33,
  /*! Returns when CNTopo does not support the dependent Driver or CNDev version. */
  CNTOPO_UNSUPPORTED = 0x4,
  /*! Indicates null pointer is invalid. */
  CNTOPO_NULL_POINTER = 0x41,
  /*! Returns when the version cannot be extracted. */
  CNTOPO_INVALID_VERSION = 0x42,
  /*! Indicates CNTopo Context has not been initialized. */
  CNTOPO_CONTEXT_NOT_INIT = 0x5,
  /*! Indicates CNTopo Context has not been created. */
  CNTOPO_CONTEXT_NOT_CREATE = 0x50,
  /*! Indicates CNTopo Query has not been initialized. */
  CNTOPO_QUERY_NOT_INIT = 0x51,
  /*! Indicates CNTopo DevSet has not been initialized. */
  CNTOPO_DEVSET_NOT_INIT = 0x52,
  /*! Indicates CNTopo topology has not been initialized. */
  CNTOPO_TOPO_NOT_INIT = 0x53,
  /*! Indicates the unsupported format of the string content. */
  CNTOPO_CONTANT_ERR_FORMAT = 0x6,
  /*! Returns when enough information cannot be obtained from CNDev. */
  CNTOPO_RET_ERR_CNDEV = 0x7,
  /*! Returns when the failure is caused by socket error. */
  CNTOPO_RET_ERR_SYS_SOCKET = 0x8,
  /*! Returns when the failure is caused by In-network compute API. */
  CNTOPO_CNLINC_ERROR = 0x9
} cntopoResult_t;

/**
 * @brief Checks CNTopo return value.
 *
 * @details If the return value is not #CNTOPO_RET_SUCCESS, the program will be
 * exited.
 */

#define CNTOPO_CHECK(call)                                 \
  do {                                                     \
    cntopoResult_t ret_code = (call);                      \
    if (ret_code != CNTOPO_RET_SUCCESS) {                  \
      printf("[%s:%d] CNTopo error, msg: %s, code: %#x\n", \
             __FILE__,                                     \
             __LINE__,                                     \
             cntopoGetErrorStr(ret_code),                  \
             ret_code);                                    \
      exit(1);                                             \
    }                                                      \
  } while (0);

/** @brief The handle of CNTopo Context. */
/** @details Obtains and saves the device information of each machine,
 * and manages the reclamation of dynamic memory. */
typedef void *cntopoContext_t;
/** @brief The handle of local machine information. */
typedef char *cntopoMachineInfo_t;
/** @brief The handle of CNTopo Query based on CNTopo Context. 
 * @details Sets the Query restrictions based on  
 * the devices information obtained by ::cntopoContext_t,
 * finds the corresponding topology according to the restrictions, such as blacklists,
 * and manages the reclamation of dynamic memory. */
typedef void *cntopoQuery_t;
/** @brief The handle of DevSet found by CNTopo Query. */
/** @details Stores a single ring or single tree device set.
 * The device set is just a set of devices,
 * unordered, without connection information or topological edges.*/
typedef void *cntopoDevSet_t;
/** @brief The handle of topology based on DevSet. */
/** @details Saves the information of non-conflict single ring topology or single tree topology.
 * The topology is ordered and edged, and you can get the exact port used by each
 * edge from the topology, which is a capability that the device set does not have. */
typedef void *cntopoTopo_t;
/** @brief Topology types supported by CNTopo.*/
typedef enum cntopoTopoType_t {
  /** Ring topology.*/
  RING,
  /** Tree topology.*/
  TREE
} cntopoTopoType_t;
/** @brief The device information.*/
typedef struct cntopoDevInfo_t {
  /** The label of a machine which owns this device.*/
  char machine_label[1024];
  /** The ordinal of this device.*/
  size_t dev_ordinal;
  /** The UUID(Universal Unique Identifier) of this device.*/
  uint64_t uuid64;
  /** The motherboard serial number of this device.*/
  uint64_t ba_sn;
  /** The serial number of this device.*/
  uint64_t mc_sn;
  /** The slot ID of this device, only supported in OAM.*/
  size_t slot_id;
} cntopoDevInfo_t;
/** @brief The link type of the port.*/
typedef enum cntopoLinkType_t {
  /** Point-to-point connection. */
  MLULINK,
  /** Point-to-switch connection. */
  MLULINKOverRoCE
} cntopoLinkType_t;
/** @brief The port type in topology.*/
typedef enum cntopoTopoPortType_t {
  /** TX(Transmitted Traffic) port in ring topology. */
  TX,
  /** RX(Received Traffic) port in ring topology. */
  RX,
  /** Port connected to leaf node in tree topology. */
  LEAF,
  /** Port connected to parent node in tree topology. */
  PARENT
} cntopoTopoPortType_t;
/** @brief The port information.*/
typedef struct cntopoPortInfo_t {
  /** The Port ID.*/
  size_t port_id;
  /** The link type of the port.*/
  cntopoLinkType_t type;
  /** The port type in topology.*/
  cntopoTopoPortType_t port_type;
} cntopoPortInfo_t;

/***********************************************************************************
 * Function prototype declaration
 ***********************************************************************************/

//Group:Error Handling
/*!
 * @details Returns a read-only string of the error code.
 *
 * @param [in] ret This error code is returned from the previous API call.
 * @retval str The pointer of a constant string.
 */
const char *cntopoGetErrorStr(cntopoResult_t ret);

//Group:Context Management

/**
 * @brief Initializes a CNTopo Context which manages all machines' information.
 *
 * @param[out] ctx Pointer to the obtained CNTopo Context handle.
 * 
 * @return
 * - ::CNTOPO_RET_SUCCESS
 *
 */
cntopoResult_t cntopoInitContext(cntopoContext_t *ctx);

//Group:Context Management

/**
 * @brief Destroys the Context to free all CNTopo inside resources including management resources.
 *
 * @param[in] ctx The handle of CNTopo Context obtained by #cntopoInitContext.
 * @return
 * - ::CNTOPO_RET_SUCCESS
 * - ::CNTOPO_CONTEXT_NOT_CREATE
 *
 */
cntopoResult_t cntopoDestroyContext(cntopoContext_t ctx);

//Group:Machine Local Information

/**
 * @brief Gets the local machine information of the JSON format string.
 *
 * @param[in] ctx The handle of CNTopo Context obtained by #cntopoInitContext.
 * @param[out] node_info The local machine information.
 * @param[out] size_bytes The length of #cntopoMachineInfo_t, ended with ``\0``.
 * @return
 * - ::CNTOPO_RET_SUCCESS
 * - ::CNTOPO_CONTEXT_NOT_CREATE
 * - ::CNTOPO_UNSUPPORTED
 * - ::CNTOPO_RET_ERR_CNDEV
 *
 */
cntopoResult_t cntopoGetLocalMachineInfo(cntopoContext_t ctx,
                                         cntopoMachineInfo_t *node_info,
                                         size_t *size_bytes);

//Group:Machine Local Information Management

/**
 * @brief Saves the local information of the machine into the specified file.
 *
 * @param[in] node_info The local information of the machine got from
 * #cntopoGetLocalMachineInfo.
 * @param[in] file_name The file name.
 * @return
 * - ::CNTOPO_RET_SUCCESS
 * - ::CNTOPO_INTERNEL_ERROR
 * - ::CNTOPO_FILE_PATHERR
 * - ::CNTOPO_CONTANT_ERR_FORMAT
 */
cntopoResult_t cntopoSaveMachineInfoToFile(cntopoMachineInfo_t node_info,
                                           const char *file_name);

//Group:Machine Local Information Management

/**
 * @brief Loads the local information of the machine from the specified file.
 *
 * @param[in] ctx The handle of CNTopo Context obtained by #cntopoInitContext.
 * @param[in] file_name The file name.
 * @param[out] node_info The local information of the machine got from file.
 * @return
 * - ::CNTOPO_RET_SUCCESS
 * - ::CNTOPO_CONTEXT_NOT_CREATE
 * - ::CNTOPO_FILE_PATHERR
 * - ::CNTOPO_FILE_EMPTY
 * - ::CNTOPO_FILE_ERR
 *
 * @note
 * - The Context is only used to manage resources in ::cntopoLoadMachineInfoFromFile. 
 * - Before finding the topology, #cntopoAddMachineInfo must be called.
 * 
 * @par Example
 * @code
 *   ...
 *   // worker node
 *   cntopoContext_t ctx;
 *   CNTOPO_CHECK(cntopoInitContext(&ctx));
 *   cntopoMachineInfo_t node_info;
 *   size_t bytes_size;
 *   std::string file_name = "local.json";
 *   CNTOPO_CHECK(cntopoGetLocalMachineInfo(ctx, &node_info, &bytes_size));
 *   CNTOPO_CHECK(cntopoSaveMachineInfoToFile(node_info, file_name.c_str()));
 *   // send the file to main node
 *   ...
 *   CNTOPO_CHECK(cntopoDestroyContext(ctx));
 *   // main node
 *   cntopoContext_t ctx;
 *   CNTOPO_CHECK(cntopoInitContext(&ctx));
 *   // got from worker node
 *   cntopoMachineInfo_t node_info;
 *   std::string file_name = "local.json";
 *   std::string machine_name = "localhost";
 *   CNTOPO_CHECK(cntopoLoadMachineInfoFromFile(ctx, file_name.c_str(), &node_info));
 *   CNTOPO_CHECK(cntopoAddMachineInfo(ctx, node_info, machine_name.c_str()));
 *   ......
 *   CNTOPO_CHECK(cntopoDestroyContext(ctx));
 *   ...
 * @endcode
 */
cntopoResult_t cntopoLoadMachineInfoFromFile(cntopoContext_t ctx,
                                             const char *file_name,
                                             cntopoMachineInfo_t *node_info);

//Group:Machine Information Management in Context

/**
 * @brief Adds machine information and label to Context.
 *
 * @param[in] ctx The handle of CNTopo context obtained by #cntopoInitContext.
 * @param[in] node_info The local machine information.
 * @param[in] machine_label The machine label to distinguish different machines.
 * @return
 * - ::CNTOPO_RET_SUCCESS
 * - ::CNTOPO_CONTEXT_NOT_CREATE
 * - ::CNTOPO_CONTANT_ERR_FORMAT
 * - ::CNTOPO_INTERNEL_ERROR
 *
 * @note
 *  - Machine information must be added before creating Query.
 *
 * @par Example
 * @code
 *   ...
 *   // worker node
 *   cntopoContext_t ctx;
 *   CNTOPO_CHECK(cntopoInitContext(&ctx));
 *   cntopoMachineInfo_t node_info;
 *   size_t bytes_size;
 *   std::string file_name = "local.json";
 *   CNTOPO_CHECK(cntopoGetLocalMachineInfo(ctx, &node_info, &bytes_size));
 *   // send node_info to main node
 *   ......
 *   CNTOPO_CHECK(cntopoDestroyContext(ctx));
 *   // main node
 *   cntopoContext_t ctx;
 *   CNTOPO_CHECK(cntopoInitContext(&ctx));
 *   // got from worker node
 *   cntopoMachineInfo_t node_info;
 *   std::string machine_name = "localhost";
 *   CNTOPO_CHECK(cntopoAddMachineInfo(ctx, node_info, machine_name.c_str()));
 *   CNTOPO_CHECK(cntopoDestroyContext(ctx));
 *   ...
 * @endcode
 */
cntopoResult_t cntopoAddMachineInfo(cntopoContext_t ctx,
                                    cntopoMachineInfo_t node_info,
                                    const char *machine_label);

//Group:Machine Information Management in Context

/**
 * @brief Clears machine information in Context.
 *
 * @param[in] ctx The handle of CNTopo Context obtained by #cntopoInitContext.
 * @return
 * - ::CNTOPO_RET_SUCCESS
 * - ::CNTOPO_CONTEXT_NOT_CREATE
 *
 * @note
 * - To query topology based on different machines without creating a new Context,
 * call ::cntopoClearMachineInfo to clear the previous machine information,
 * and call ::cntopoAddMachineInfo to add the new machine information to the Context.
 * - Call ::cntopoCreateQuery to create a new Query to query the topology,
 * if the device information in the Context changes.
 * The previous Query still retains the previous device information. 
 *
 */
cntopoResult_t cntopoClearMachineInfo(cntopoContext_t ctx);

//Group:Query Management

/**
 * @brief Creates CNTopo Query based on Context.
 *
 * @param[in] ctx The handle of CNTopo Context obtained by #cntopoInitContext.
 * @param[out] query_handle The handle of CNTopo Query.
 * @return
 * - ::CNTOPO_RET_SUCCESS
 * - ::CNTOPO_CONTEXT_NOT_CREATE
 * - ::CNTOPO_CONTEXT_NOT_INIT
 *
 * @details The Query manages connection resources which are used to find DevSet.
 *
 * @par Example
 * @code
 *   ...
 *   CNTOPO_CHECK(cntopoCreateQuery(ctx, &query_handle));
 *   size_t dev_num = 4;
 *   char* machine_label = const_cast<char*>("localhost");
 *   CNTOPO_CHECK(cntopoSetDevNumFilter(query_handle, machine_label, dev_num));
 *   CNTOPO_CHECK(
 *           cntopoSetBlacklistDevOrdinal(query_handle, machine_label, 2));
 *   ...
 *   CNTOPO_CHECK(cntopoDestroyQuery(query_handle));
 *   ...
 *   CNTOPO_CHECK(cntopoCreateQuery(ctx, &query_handle));
 *   // different query configuration
 *   dev_num = 6;
 *   char* machine_label = const_cast<char*>("localhost");
 *   CNTOPO_CHECK(cntopoSetDevNumFilter(query_handle, machine_label, dev_num));
 *   CNTOPO_CHECK(
 *           cntopoSetBlacklistDevOrdinal(query_handle, machine_label, 4));
 *   CNTOPO_CHECK(
 *           cntopoSetBlacklistDevOrdinal(query_handle, machine_label, 5));
 *   ...
 *   CNTOPO_CHECK(cntopoDestroyQuery(query_handle));
 * @endcode
 * 
 * @note 
 * - Different query configuration must use different ::cntopoQuery_t.
 */
cntopoResult_t cntopoCreateQuery(cntopoContext_t ctx, cntopoQuery_t *query_handle);

//Group:Query Management

/**
 * @brief Destroys CNTopo Query to free all Query management resources.
 *
 * @param[in] query_handle The handle of CNTopo Query obtain by #cntopoCreateQuery.
 * @return
 * - ::CNTOPO_RET_SUCCESS
 * - ::CNTOPO_CONTEXT_NOT_INIT
 *
 */
cntopoResult_t cntopoDestroyQuery(cntopoQuery_t query_handle);

//Group:Query Configuration

/**
 * @brief Sets the number of devices used in the DevSet of the specified machine.
 *
 * @param[in] query_handle The handle of CNTopo Query obtain by #cntopoCreateQuery.
 * @param[in] machine_label The label of the machine corresponding to
 * #cntopoAddMachineInfo.
 * @param[in] num_dev The used device number of machine_label.
 * @return
 * - ::CNTOPO_RET_SUCCESS
 * - ::CNTOPO_QUERY_NOT_INIT
 * 
 * @par Example
 * @code
 *   ...
 *   CNTOPO_CHECK(cntopoCreateQuery(ctx, &query_handle));
 *   size_t dev_num = 4;
 *   char* machine_label = const_cast<char*>("localhost");
 *   CNTOPO_CHECK(cntopoSetDevNumFilter(query_handle, machine_label, dev_num));
 *   ...
 *   CNTOPO_CHECK(cntopoDestroyQuery(query_handle));
 *   ...
 * @endcode
 */
cntopoResult_t cntopoSetDevNumFilter(cntopoQuery_t query_handle,
                                     const char *machine_label,
                                     uint32_t num_dev);

//Group:Query Configuration

/**
 * @brief Adds the device ordinal of the specified machine to the blacklist.
 *
 * @param[in] query_handle The handle of CNTopo Query obtain by #cntopoCreateQuery.
 * @param[in] machine_label The label of the machine corresponding to
 * #cntopoAddMachineInfo.
 * @param[in] dev_ordinal The device ordinal in the blacklist.
 * @return
 * - ::CNTOPO_RET_SUCCESS
 * - ::CNTOPO_QUERY_NOT_INIT
 * @par Example
 * @code
 *   ...
 *   CNTOPO_CHECK(cntopoCreateQuery(ctx, &query_handle));
 *   char* machine_label = const_cast<char*>("localhost");
 *   CNTOPO_CHECK(
 *         cntopoSetBlacklistDevOrdinal(query_handle, machine_label, 2));
 *   ...
 *   CNTOPO_CHECK(cntopoDestroyQuery(query_handle));
 *   ...
 * @endcode
 */
cntopoResult_t cntopoSetBlacklistDevOrdinal(cntopoQuery_t query_handle,
                                            const char *machine_label,
                                            size_t dev_ordinal);

//Group:Query Configuration

/**
 * @brief Adds the device UUID of the specified machine to the blacklist.
 *
 * @param[in] query_handle The handle of CNTopo Query obtain by #cntopoCreateQuery.
 * @param[in] machine_label The label of the machine corresponding to
 * #cntopoAddMachineInfo.
 * @param[in] uuid The device UUID in the blacklist.
 * @return
 * - ::CNTOPO_RET_SUCCESS
 * - ::CNTOPO_QUERY_NOT_INIT
 * @par Example
 * @code
 *   ...
 *   CNTOPO_CHECK(cntopoCreateQuery(ctx, &query_handle));
 *   char* machine_label = const_cast<char*>("localhost");
 *   CNTOPO_CHECK(
 *         cntopoSetBlacklistUUID(query_handle, machine_label, 71605815345168));
 *   ...
 *   CNTOPO_CHECK(cntopoDestroyQuery(query_handle));
 *   ...
 * @endcode
 */
cntopoResult_t cntopoSetBlacklistUUID(cntopoQuery_t query_handle,
                                      const char *machine_label,
                                      uint64_t uuid);

//Group:Query Configuration

/**
 * @brief Adds the device ordinal of the specified machine to the whitelist.
 *
 * @param[in] query_handle The handle of CNTopo Query obtain by #cntopoCreateQuery.
 * @param[in] machine_label The label of the machine corresponding to
 * #cntopoAddMachineInfo.
 * @param[in] dev_ordinal The device ordinal in the whitelist.
 * @return
 * - ::CNTOPO_RET_SUCCESS
 * - ::CNTOPO_QUERY_NOT_INIT
 * 
 * @par Example
 * @code
 *   ...
 *   CNTOPO_CHECK(cntopoCreateQuery(ctx, &query_handle));
 *   char* machine_label = const_cast<char*>("localhost");
 *   CNTOPO_CHECK(
 *         cntopoSetWhitelistDevOrdinal(query_handle, machine_label, 3));
 *   ...
 *   CNTOPO_CHECK(cntopoDestroyQuery(query_handle));
 *   ...
 * @endcode
 */
cntopoResult_t cntopoSetWhitelistDevOrdinal(cntopoQuery_t query_handle,
                                            const char *machine_label,
                                            size_t dev_ordinal);

//Group:Query Configuration

/**
 * @brief Adds the device UUID of the specified machine to the whitelist.
 *
 * @param[in] query_handle The handle of CNTopo Query obtain by #cntopoCreateQuery.
 * @param[in] machine_label The label of the machine corresponding to
 * #cntopoAddMachineInfo.
 * @param[in] uuid The device UUID in the whitelist.
 * @return
 * - ::CNTOPO_RET_SUCCESS
 * - ::CNTOPO_QUERY_NOT_INIT
 * @par Example
 * @code
 *   ...
 *   CNTOPO_CHECK(cntopoCreateQuery(ctx, &query_handle));
 *   char* machine_label = const_cast<char*>("localhost");
 *   CNTOPO_CHECK(
 *         cntopoSetWhitelistUUID(query_handle, machine_label, 71605815345168));
 *   ...
 *   CNTOPO_CHECK(cntopoDestroyQuery(query_handle));
 *   ...
 * @endcode
 */
cntopoResult_t cntopoSetWhitelistUUID(cntopoQuery_t query_handle,
                                      const char *machine_label,
                                      uint64_t uuid);

//Group:DevSet Information Management

/**
 * @brief Finds DevSet according to input topology type and maximum number of topology.
 *
 * @param[in] query_handle The handle of CNTopo Query obtain by #cntopoCreateQuery.
 * @param[in] topo_type The topology type. Only RING and TREE are supported.
 * @param[in] max_topo_num The maximum number of topology.
 * @param[out] dev_sets The found DevSet list.
 * @param[out] num_dev_set The number of DevSet actually found.
 * @return
 * - ::CNTOPO_RET_SUCCESS
 * - ::CNTOPO_QUERY_NOT_INIT
 *
 * @note 
 * - ``num_dev_set`` might be less than ``max_topo_num``.
 * 
 * @par Example
 * @code
 *   ...
 *   CNTOPO_CHECK(cntopoCreateQuery(ctx, &query_handle));
 *   ...
 *   cntopoDevSet_t* dev_sets;
 *   size_t num_dev_set;
 *   CNTOPO_CHECK(cntopoFindDevSets(query_handle, RING, 1, &dev_sets, &num_dev_set));
 *   ...
 *   CNTOPO_CHECK(cntopoDestroyQuery(query_handle));
 *   ...
 * @endcode
 */
cntopoResult_t cntopoFindDevSets(cntopoQuery_t query_handle,
                                 cntopoTopoType_t topo_type,
                                 size_t max_topo_num,
                                 cntopoDevSet_t **dev_sets,
                                 size_t *num_dev_set);

//Group:DevSet Information Management

/**
 * @brief Gets the number of devices in DevSet.
 *
 * @param[in] dev_set The found DevSet in #cntopoFindDevSets.
 * @param[out] size The number of DevSet actually found.
 * @return
 * - ::CNTOPO_RET_SUCCESS
 * - ::CNTOPO_DEVSET_NOT_INIT
 * 
 * @par Example
 * @code
 *   ...
 *   CNTOPO_CHECK(cntopoCreateQuery(ctx, &query_handle));
 *   ...
 *   cntopoDevSet_t* dev_sets;
 *   size_t num_dev_set;
 *   CNTOPO_CHECK(cntopoFindDevSets(query_handle, RING, 1, &dev_sets, &num_dev_set));
 *   for (size_t i = 0; i < num_dev_set; i++) {
 *     size_t dev_size;
 *     CNTOPO_CHECK(cntopoGetDevSetSize(dev_sets[i], &dev_size));
 *   }
 *   ...
 *   CNTOPO_CHECK(cntopoDestroyQuery(query_handle));
 *   ...
 * @endcode
 */
cntopoResult_t cntopoGetDevSetSize(cntopoDevSet_t dev_set, size_t *size);

//Group:DevSet Information Management

/**
 * @brief Gets the information about each device in DevSet.
 *
 * @param[in] dev_set The found DevSet in #cntopoFindDevSets.
 * @param[in] dev_index The index of device in DevSet.
 * @param[out] dev_info The information of device.
 * @return
 * - ::CNTOPO_RET_SUCCESS
 * - ::CNTOPO_DEVSET_NOT_INIT
 * 
 * @par Example
 * @code
 *   ...
 *   CNTOPO_CHECK(cntopoCreateQuery(ctx, &query_handle));
 *   ...
 *   cntopoDevSet_t* dev_sets;
 *   size_t num_dev_set;
 *   CNTOPO_CHECK(cntopoFindDevSets(query_handle, RING, 1, &dev_sets, &num_dev_set));
 *   for (size_t i = 0; i < num_dev_set; i++) {
 *     size_t dev_size;
 *     CNTOPO_CHECK(cntopoGetDevSetSize(dev_sets[i], &dev_size));
 *     for (size_t dev_index = 0; dev_index < dev_size; dev_index++) {
 *       cntopoDevInfo_t dev_info;
 *       CNTOPO_CHECK(cntopoGetDevInfoFromDevSet(dev_sets[i], dev_index, &dev_info));
 *     }
 *   }
 *   ...
 *   CNTOPO_CHECK(cntopoDestroyQuery(query_handle));
 *   ...
 * @endcode
 */
cntopoResult_t cntopoGetDevInfoFromDevSet(cntopoDevSet_t dev_set,
                                          size_t dev_index,
                                          cntopoDevInfo_t *dev_info);

//Group:Topology Information Management

/**
 * @brief Finds topology according to input DevSet and topology type.
 *
 * @param[in] dev_set The found DevSet in #cntopoFindDevSets.
 * @param[in] topo_type The topology type. Only RING and TREE are supported.
 * @param[out] topos The found topo list.
 * @param[out] num_topo The number of topo actually found.
 * @return
 * - ::CNTOPO_RET_SUCCESS
 * - ::CNTOPO_DEVSET_NOT_INIT
 * @par Example
 * @code
 *   ...
 *   CNTOPO_CHECK(cntopoCreateQuery(ctx, &query_handle));
 *   ...
 *   cntopoDevSet_t* dev_sets;
 *   size_t num_dev_set;
 *   CNTOPO_CHECK(cntopoFindDevSets(query_handle, RING, 1, &dev_sets, &num_dev_set));
 *   for (size_t i = 0; i < num_dev_set; i++) {
 *     size_t dev_size;
 *     CNTOPO_CHECK(cntopoGetDevSetSize(dev_sets[i], &dev_size));
 *     for (size_t dev_index = 0; dev_index < dev_size; dev_index++) {
 *       cntopoDevInfo_t dev_info;
 *       CNTOPO_CHECK(cntopoGetDevInfoFromDevSet(dev_sets[i], dev_index, &dev_info));
 *     }
 *     cntopoTopo_t* topos;
 *     size_t num_topo;
 *     CNTOPO_CHECK(cntopoFindTopos(dev_sets[i], RING, &topos, &num_topo));
 *   }
 *   ...
 *   CNTOPO_CHECK(cntopoDestroyQuery(query_handle));
 *   ...
 * @endcode
 */
cntopoResult_t cntopoFindTopos(cntopoDevSet_t dev_set,
                               cntopoTopoType_t topo_type,
                               cntopoTopo_t **topos,
                               size_t *num_topo);

//Group:Topology Information Management

/**
 * @brief Gets the information about each device in topology.
 *
 * @param[in] topo The found topo in #cntopoFindTopos.
 * @param[in] dev_index The index of device in DevSet.
 * @param[out] dev_info The information of device.
 * @param[out] port_infos The information of used port in topo.
 * @param[out] num_port_info The number of port.
 * @return
 * - ::CNTOPO_RET_SUCCESS
 * - ::CNTOPO_TOPO_NOT_INIT
 *
 * @note 
 * - Each device might have different ``num_port_info``. For example, in tree topology, if
 * one node has one parent node and no leaf node, the value of ``num_port_info`` is ``1``; if
 * one node has two leaf nodes and no parent node, the value of ``num_port_info`` is ``2``.
 * 
 * @par Example
 * @code
 *   ...
 *   CNTOPO_CHECK(cntopoCreateQuery(ctx, &query_handle));
 *   ...
 *   cntopoDevSet_t* dev_sets;
 *   size_t num_dev_set;
 *   CNTOPO_CHECK(cntopoFindDevSets(query_handle, RING, 1, &dev_sets, &num_dev_set));
 *   for (size_t i = 0; i < num_dev_set; i++) {
 *     size_t dev_size;
 *     CNTOPO_CHECK(cntopoGetDevSetSize(dev_sets[i], &dev_size));
 *     for (size_t dev_index = 0; dev_index < dev_size; dev_index++) {
 *       cntopoDevInfo_t dev_info;
 *       CNTOPO_CHECK(cntopoGetDevInfoFromDevSet(dev_sets[i], dev_index, &dev_info));
 *     }
 *     cntopoTopo_t* topos;
 *     size_t num_topo;
 *     CNTOPO_CHECK(cntopoFindTopos(dev_sets[i], RING, &topos, &num_topo));
 *     for (size_t topo_index = 0; topo_index < num_topo; topo_index++) {
 *       for (size_t dev_index = 0; dev_index < dev_size; dev_index++) {
 *         cntopoDevInfo_t dev_info;
 *         cntopoPortInfo_t* port_infos;
 *         size_t port_size;
 *         CNTOPO_CHECK(cntopoGetNodeFromTopo(
 *             topos[topo_index], dev_index, &dev_info, &port_infos, &port_size));
 *         for (size_t port_index = 0; port_index < port_size; port_index++) {
 *           printf("port id: %ld, link type: %d, port type: %d\n",
 *                  port_infos[port_index].port_id,
 *                  port_infos[port_index].type,
 *                  port_infos[port_index].port_type);
 *         }
 *       }
 *     }
 *   }
 *   ...
 *   CNTOPO_CHECK(cntopoDestroyQuery(query_handle));
 *   ...
 * @endcode
 * 
 * @par Example
 * @code
 *   ...
 *   cntopoContext_t ctx;
 *   CNTOPO_CHECK(cntopoInitContext(&ctx));
 *   char* machine_label = const_cast<char*>("localhost");
 *   cntopoMachineInfo_t node_infos;
 *   CNTOPO_CHECK(cntopoLoadMachineInfoFromFile(ctx, "local.json", &node_infos));
 *   CNTOPO_CHECK(cntopoAddMachineInfo(ctx, node_infos, machine_label));
 *   cntopoQuery_t query_handle;
 *   CNTOPO_CHECK(cntopoCreateQuery(ctx, &query_handle));
 *   size_t dev_num = 4;
 *   CNTOPO_CHECK(cntopoSetDevNumFilter(query_handle, machine_label, dev_num));
 *   size_t black_list[2] = {0, 3};
 *   for (int j = 0; j < 2; j++) {
 *     CNTOPO_CHECK(
 *         cntopoSetBlacklistDevOrdinal(query_handle, machine_label, black_list[j]));
 *   }
 *   cntopoDevSet_t* dev_sets;
 *   size_t num_dev_set;
 *   CNTOPO_CHECK(cntopoFindDevSets(query_handle, RING, 1, &dev_sets, &num_dev_set));
 *   printf("num_dev_set: %ld\n", num_dev_set);
 *   for (size_t i = 0; i < num_dev_set; i++) {
 *     size_t dev_size;
 *     CNTOPO_CHECK(cntopoGetDevSetSize(dev_sets[i], &dev_size));
 *     printf("dev_size: %ld\n", dev_size);
 *     for (size_t dev_index = 0; dev_index < dev_size; dev_index++) {
 *       cntopoDevInfo_t dev_info;
 *       CNTOPO_CHECK(cntopoGetDevInfoFromDevSet(dev_sets[i], dev_index, &dev_info));
 *       printf("ordinal: %ld, uuid: %ld, label: %s\n",
 *              dev_info.dev_ordinal,
 *              dev_info.uuid64,
 *              dev_info.machine_label);
 *     }
 *     cntopoTopo_t* topos;
 *     size_t num_topo;
 *     CNTOPO_CHECK(cntopoFindTopos(dev_sets[i], RING, &topos, &num_topo));
 *     printf("num_topo: %ld\n", num_topo);
 *     for (size_t topo_index = 0; topo_index < num_topo; topo_index++) {
 *       printf("dev_size: %ld\n", dev_size);
 *       for (size_t dev_index = 0; dev_index < dev_size; dev_index++) {
 *         cntopoDevInfo_t dev_info;
 *         cntopoPortInfo_t* port_infos;
 *         size_t port_size;
 *         CNTOPO_CHECK(cntopoGetNodeFromTopo(
 *             topos[topo_index], dev_index, &dev_info, &port_infos, &port_size));
 *         printf("ordinal: %ld, uuid: %ld, label: %s\n",
 *                dev_info.dev_ordinal,
 *                dev_info.uuid64,
 *                dev_info.machine_label);
 *         printf("port_size: %ld\n", port_size);
 *         for (size_t port_index = 0; port_index < port_size; port_index++) {
 *           printf("port id: %ld, link type: %d, port type: %d\n",
 *                  port_infos[port_index].port_id,
 *                  port_infos[port_index].type,
 *                  port_infos[port_index].port_type);
 *         }
 *       }
 *     }
 *   }
 *   CNTOPO_CHECK(cntopoDestroyQuery(query_handle));
 *   CNTOPO_CHECK(cntopoDestroyContext(ctx));
 *   ...
 * @endcode
 */
cntopoResult_t cntopoGetNodeFromTopo(cntopoTopo_t topo,
                                     size_t dev_index,
                                     cntopoDevInfo_t *dev_info,
                                     cntopoPortInfo_t **port_infos,
                                     size_t *num_port_info);

//Group:Version Control

/**
 * @brief Gets the current CNTopo library version to check if CNTopo satisfies the dependency.
 *
 * @param[out] major Pointer to the major version id.
 * @param[out] minor Pointer to the minor version id.
 * @param[out] patch Pointer to the patch version id.
 * @return
 * - ::CNTOPO_RET_SUCCESS
 * - ::CNTOPO_INVALID_VERSION
 * - ::CNTOPO_INTERNEL_ERROR
 *
 * @par Example
 * Get the version of the current CNTopo.
 * @code{.cpp}
 * int major, minor, patch;
 * CNTOPO_CHECK(cntopoGetLibVersion(&major, &minor, &patch));
 * @endcode
 */
cntopoResult_t cntopoGetLibVersion(int *major, int *minor, int *patch);

#if defined(__cplusplus)
}
#endif  /*__cplusplus*/
#endif  // CNTOPO_H_ // NOLINT
