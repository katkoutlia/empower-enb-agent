/* Copyright (c) 2017 Kewin Rausch
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 *  Empower Agent Protocols definitions
 *
 *  Version 1.0
 */

#ifndef __EMAGE_PROTOCOLS_v1_H
#define __EMAGE_PROTOCOLS_v1_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifdef EMPOWER_PROTOCOL_VERS
#error "Trying to mix different version of the Empower protocols!"
#endif /* EMPOWER_PROTOCOL_VERS */

#define EMPOWER_PROTOCOL_VERS            1

#define EP_SUCCESS                       0
#define EP_ERROR                        -1
#define EP_WRONG_VERSION                -2

/* Size in bytes of the message prologue, which contains the next message size
 */
#define EP_PROLOGUE_SIZE                 4

#include "ephdr.h"
#include "eptype.h"
#include "epdir.h"

#include "epsingle.h"
#include "epsched.h"
#include "eptrig.h"

#include "ephello.h"
#include "epenbcap.h"
#include "epcelcap.h"
#include "epuerep.h"
#include "epuemeas.h"
#include "epmacrep.h"
#include "epho.h"

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EMAGE_PROTOCOLS_v1_H */
