/*************************************************************************
 * Copyright (C) [2019] by Cambricon, Inc. All rights reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
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
 *************************************************************************/

#include <stddef.h>
#include <dlfcn.h>
#include "cndev_dl.h"
#include "./include/cndev.h"

#define DLSYM(x, sym)                           \
do {                                            \
    dlerror();                                  \
    x = dlsym(handle, #sym);                    \
    if (dlerror() != NULL) {                    \
        return (CNDEV_ERROR_FUNCTION_NOT_FOUND); \
    }                                           \
} while (0)

typedef cndevRet_t (*cndevSym_t)();

static void *handle;

cndevRet_t CNDEV_DL(cndevInit)(void)
{
    handle = dlopen("libcndev.so", RTLD_LAZY | RTLD_GLOBAL);
    if (handle == NULL) {
        return (CNDEV_ERROR_UNINITIALIZED);
    }
    return (cndevInit(0));
}

cndevRet_t CNDEV_DL(cndevShutdown)(void)
{
    cndevRet_t r = cndevRelease();
    if (r != CNDEV_SUCCESS) {
        return (r);
    }
    return (dlclose(handle) ? CNDEV_ERROR_UNKNOWN : CNDEV_SUCCESS);
}

cndevRet_t CNDEV_DL(cndevRelease)(void)
{
    return CNDEV_SUCCESS;
}
