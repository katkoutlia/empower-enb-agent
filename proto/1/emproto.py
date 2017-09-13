#!/usr/bin/env python3
#
# Copyright (c) 2017 Kewin Rausch
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied. See the License for the
# specific language governing permissions and limitations
# under the License.

"""Empower Protocol wrapper for Python"""

import ctypes

#
# Import EmPOWER Protocols library
#

_emproto = ctypes.CDLL('libemproto.so')

#
# Setup signature of the procedures imported from the library
#

_emproto.epp_msg_type.agrgtypes        = (ctypes.Array, ctypes.c_uint32)
_emproto.epf_inject_seq.agrgtypes      = (ctypes.Array, ctypes.c_uint32, ctypes.c_uint32)
_emproto.epp_trigger_dir.agrgtypes     = (ctypes.Array, ctypes.c_uint32)
_emproto.epp_schedule_type.agrgtypes   = (ctypes.Array, ctypes.c_uint32)
_emproto.epp_single_dir.agrgtypes      = (ctypes.Array, ctypes.c_uint32)
_emproto.epp_single_type.agrgtypes     = (ctypes.Array, ctypes.c_uint32)
_emproto.epp_trigger_dir.agrgtypes     = (ctypes.Array, ctypes.c_uint32)
_emproto.epp_trigger_type.agrgtypes    = (ctypes.Array, ctypes.c_uint32)

_emproto.epf_single_hello_rep.argtypes = (ctypes.Array, ctypes.c_uint32, ctypes.c_uint32, ctypes.c_uint16, ctypes.c_uint32, ctypes.c_uint32)
_emproto.epf_single_hello_req.argtypes = (ctypes.Array, ctypes.c_uint32, ctypes.c_uint32, ctypes.c_uint16, ctypes.c_uint32, ctypes.c_uint32)

#
# Enumerations
# They must be maintained equal to the one present in the library headers
#

# First-level message type
class EP_MSG_TYPE:
    INVALID          = 0
    SINGLE           = 1
    SCHEDULE         = 2
    TRIGGER          = 3
    EXTENDED         = 255

# Direction fo the message
class EP_DIR_TYPE:
    REQUEST          = 0
    REPLY            = 1

# Type of operation to perform
class EP_OP_TYPE:
    REM              = 0
    ADD              = 1

# Schedule-event message types
class EP_SCH_TYPE:
    INVALID          = 0
    HELLO            = 1
    EXTENDED         = 255

# Single-event message types
class EP_SIN_TYPE:
    INVALID          = 0
    HELLO            = 1
    EXTENDED         = 255

# Trigger-event message types
class EP_TR_TYPE:
    INVALID          = 0
    HELLO            = 1
    EXTENDED         = 255

#
# EmPOWER Protocols public definitions
#

class Emproto(object):
    # Size of the prologue field which contains the next message size
    EP_PROLOGUE_SIZE = 4

    #
    # Master header
    #

    def epp_msg_type(buf, size):
        global _emproto
        _emproto.epp_msg_type(buf, size)

    def epf_inject_seq(buf, size):
        global _emproto
        _emproto.epf_inject_seq(buf, size)    

    #
    # Schedule-event messages
    #

    def epp_schedule_dir(buf, size):
        global _emproto
        _emproto.epp_schedule_dir(buf, size)

    def epp_schedule_type(buf, size):
        global _emproto
        _emproto.epp_schedule_type(buf, size)

    #
    # Single-event messages
    #

    def epp_single_dir(buf, size):
        global _emproto
        _emproto.epp_single_dir(buf, size)

    def epp_single_type(buf, size):
        global _emproto
        _emproto.epp_single_type(buf, size)

    #
    # Trigger-event messages
    #

    def epp_trigger_dir(buf, size):
        global _emproto
        _emproto.epp_trigger_dir(buf, size)

    def epp_trigger_type(buf, size):
        global _emproto
        _emproto.epp_trigger_type(buf, size)
        
    #
    # Hello message
    #

    def epf_single_hello_rep(buf, size, enb_id, cell_id, mod_id, hello_id):
        global _emproto
        return _emproto.epf_single_hello_rep(buf, size, enb_id, cell_id, mod_id, hello_id)

    def epf_single_hello_req(buf, size, enb_id, cell_id, mod_id, hello_id):
        global _emproto
        return _emproto.epf_single_hello_req(buf, size, enb_id, cell_id, mod_id, hello_id)
