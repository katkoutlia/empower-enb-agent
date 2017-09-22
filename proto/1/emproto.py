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
# Enumerations
# They must be maintained equal to the one present in the library headers
#

# Cell supported capabilities
class EP_CCAP_TYPE:
    NOTHING          = 0

# eNB supported capabilities
class EP_ECAP_TYPE:
    NOTHING          = 0
    UE_REPORT        = 1
    UE_MEASURE       = 2

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
class EP_OPERATION_TYPE:
    UNSPECIFIED      = 0
    SUCCESS          = 1
    FAILURE          = 2
    NOT_SUPPORTED    = 3
    REM              = 4
    ADD              = 5

# Schedule-event message types
class EP_SCH_TYPE:
    INVALID          = 0
    HELLO            = 1
    EXTENDED         = 255

# Single-event message types
class EP_SIN_TYPE:
    INVALID          = 0
    HELLO            = 1
    ECAP             = 2
    CCAP             = 3
    EXTENDED         = 255

# Trigger-event message types
class EP_TR_TYPE:
    INVALID          = 0
    HELLO            = 1
    UE_REPORT        = 2
    EXTENDED         = 255

#
# EmPOWER protocols opaque structures (to not expose API with protocol internal organization)
#

class ep_cell_details(ctypes.Structure):
    _fields_ = [
        ("DL_earfcn", ctypes.c_uint16),
        ("UL_earfcn", ctypes.c_uint16),
        ("DL_prbs",   ctypes.c_uint8),
        ("UL_prbs",   ctypes.c_uint8)]

class ep_ue_details(ctypes.Structure):
    _fields_ = [
        ("pci",  ctypes.c_uint16),
        ("plmn", ctypes.c_uint32),
        ("rnti", ctypes.c_uint16),
        ("imsi", ctypes.c_uint64)]

class ep_ue_measure(ctypes.Structure):
    _fields_ = [
        ("rnti", ctypes.c_uint16),
        ("rsrp", ctypes.c_uint16),
        ("rsrq", ctypes.c_uint16)]

#
# EmPOWER Protocols public definitions
#

class Emproto(object):
    # Size of the prologue field which contains the next message size
    EP_PROLOGUE_SIZE = 4

    # Error code returned while parsing an incoming message
    EP_SUCCESS       = 0
    EP_ERROR         = -1
    EP_WRONG_VERSION = -2
    
    #
    # Master header
    #

    def epf_head(buf, size, m_type, enb_id, cell_id, mod_id):
        global _emproto
        return _emproto.epp_head(buf, size, m_type, enb_id, cell_id, mod_id)

    def epp_head(buf, size, m_type, enb_id, cell_id, mod_id):
        global _emproto
        return _emproto.epp_head(buf, size, m_type, enb_id, cell_id, mod_id)

    def epp_msg_type(buf, size):
        global _emproto
        return _emproto.epp_msg_type(buf, size)

    def epp_seq(buf, size):
        global _emproto
        return _emproto.epp_seq(buf, size)

    def epf_seq(buf, size, seq):
        global _emproto
        return _emproto.epf_seq(buf, size, seq)

    #
    # Schedule-event messages
    #

    def epp_schedule_dir(buf, size):
        global _emproto
        return _emproto.epp_schedule_dir(buf, size)

    def epp_schedule_type(buf, size):
        global _emproto
        return _emproto.epp_schedule_type(buf, size)

    #
    # Single-event messages
    #

    def epp_single_dir(buf, size):
        global _emproto
        return _emproto.epp_single_dir(buf, size)

    def epp_single_type(buf, size):
        global _emproto
        return _emproto.epp_single_type(buf, size)

    #
    # Trigger-event messages
    #

    def epp_trigger_dir(buf, size):
        global _emproto
        return _emproto.epp_trigger_dir(buf, size)

    def epp_trigger_op(buf, size):
        global _emproto
        return _emproto.epp_trigger_op(buf, size)

    def epp_trigger_type(buf, size):
        global _emproto
        return _emproto.epp_trigger_type(buf, size)
    
    #
    # Hello message
    #

    def epf_single_hello_rep(buf, size, enb_id, cell_id, mod_id, hello_id):
        global _emproto
        return _emproto.epf_single_hello_rep(buf, size, enb_id, cell_id, mod_id, hello_id)

    def epf_single_hello_req(buf, size, enb_id, cell_id, mod_id, hello_id):
        global _emproto
        return _emproto.epf_single_hello_req(buf, size, enb_id, cell_id, mod_id, hello_id)

    #
    # Cell capabilities message
    #

    def epf_single_ccap_rep(buf, size, enb_id, cell_id, mod_id, cap_mask, cell_d):
        global _emproto
        return _emproto.epf_single_ccap_rep(buf, size, enb_id, cell_id, mod_id, cap_mask, cell_d)
    
    def epp_single_ccap_rep(buf, size, cap_mask, cell_d):
        global _emproto
        return _emproto.epp_single_ccap_rep(buf, size, cap_mask, cell_d)

    def epf_single_ccap_req(buf, size, enb_id, cell_id, mod_id):
        global _emproto
        return _emproto.epf_single_ccap_req(buf, size, enb_id, cell_id, mod_id)
    
    def epp_single_ccap_req(buf, size):
        global _emproto
        return _emproto.epp_single_ccap_req(buf, size)

    #
    # eNB capabilities message
    #

    def epf_single_ecap_rep(buf, size, enb_id, cell_id, mod_id, cap_mask, cells, nof_cells):
        global _emproto
        return _emproto.epf_single_ecap_rep(buf, size, enb_id, cell_id, mod_id, cap_mask, cells, nof_cells)
    
    def epp_single_ecap_rep(buf, size, cap_mask, cells, nof_cells):
        global _emproto
        return _emproto.epp_single_ecap_rep(buf, size, cap_mask, cells, nof_cells)

    def epf_single_ecap_req(buf, size, enb_id, cell_id, mod_id):
        global _emproto
        return _emproto.epf_single_ecap_req(buf, size, enb_id, cell_id, mod_id)
    
    def epp_single_ecap_req(buf, size):
        global _emproto
        return _emproto.epp_single_ecap_req(buf, size)

    #
    # UE report message
    #

    def epf_trigger_uerep_rep_fail(buf, size, enb_id, cell_id, mod_id):
        global _emproto
        return _emproto.epf_trigger_uerep_rep_fail(buf, size, enb_id, cell_id, mod_id)
    
    def epf_trigger_uerep_rep(buf, size, cap_mask, cells, nof_ues, ues):
        global _emproto
        return _emproto.epf_trigger_uerep_rep(buf, size, cap_mask, nof_ues, ues)

    def epp_trigger_uerep_rep(buf, size, nof_uesm, ues):
        global _emproto
        return _emproto.epp_trigger_uerep_rep(buf, size, nof_uesm, ues)
    
    def epf_trigger_uerep_req(buf, size, enb_id, cell_id, mod_id, op):
        global _emproto
        return _emproto.epf_trigger_uerep_req(buf, size, enb_id, cell_id, mod_id, op)

    def epp_trigger_uerep_req(buf, size):
        global _emproto
        return _emproto.epf_trigger_uerep_req(buf, size)

    #
    # UE measurements message
    #

    def epf_trigger_uemeas_rep_fail(buf, size, enb_id, cell_id, mod_id):
        global _emproto
        return _emproto.epf_trigger_uemeas_rep_fail(buf, size, enb_id, cell_id, mod_id)
    
    def epf_trigger_uemeas_rep(buf, size, cap_mask, cells, nof_ues, ues):
        global _emproto
        return _emproto.epf_trigger_uemeas_rep(buf, size, cap_mask, nof_ues, ues)

    def epp_trigger_uemeas_rep(buf, size, nof_uesm, ues):
        global _emproto
        return _emproto.epp_trigger_uemeas_rep(buf, size, nof_uesm, ues)
    
    def epf_trigger_uemeas_req(buf, size, enb_id, cell_id, mod_id, op):
        global _emproto
        return _emproto.epf_trigger_uemeas_req(buf, size, enb_id, cell_id, mod_id, op)

    def epp_trigger_uemeas_req(buf, size):
        global _emproto
        return _emproto.epf_trigger_uemeas_req(buf, size)
