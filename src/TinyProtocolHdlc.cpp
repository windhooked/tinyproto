/*
    Copyright 2019-2020 (C) Alexey Dynda

    This file is part of Tiny Protocol Library.

    Protocol Library is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Protocol Library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Protocol Library.  If not, see <http://www.gnu.org/licenses/>.
*/

#if defined(ARDUINO)
#   if ARDUINO >= 100
    #include "Arduino.h"
#   else
    #include "WProgram.h"
#   endif
#endif

#include "TinyProtocolHdlc.h"

namespace Tiny
{
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

int ProtoHdlc::onReceiveInternal(void *handle, void *pdata, int size)
{
    (reinterpret_cast<ProtoHdlc*>(handle))->onReceive((uint8_t *)pdata, size);
    return 0;
}

int ProtoHdlc::onSendInternal(void *handle, const void *pdata, int size)
{
    (reinterpret_cast<ProtoHdlc*>(handle))->onSend((const uint8_t *)pdata, size);
    return 0;
}

void ProtoHdlc::begin(write_block_cb_t writecb,
                     read_block_cb_t readcb)
{
    m_data.send_tx          = writecb;
    m_data.on_frame_read    = onReceiveInternal;
    m_data.on_frame_sent    = onSendInternal;
    m_data.rx_buf           = m_buffer;
    m_data.rx_buf_size      = m_bufferSize;
    m_data.crc_type         = m_crc;
    m_data.multithread_mode = false;
    m_data.user_data        = this;

    m_handle = hdlc_init( &m_data  );
}

void ProtoHdlc::begin()
{
    begin(nullptr, nullptr);
}

void ProtoHdlc::end()
{
    if ( m_bufferSize == 0 ) return;
    hdlc_close( m_handle );
}

int ProtoHdlc::write(char* buf, int size)
{
    return hdlc_send( m_handle, buf, size, 0 );
}

int ProtoHdlc::write(IPacket &pkt)
{
    return write( (char *)pkt.m_buf, pkt.m_len );
}

int ProtoHdlc::run_rx(const void *data, int len)
{
    return hdlc_run_rx( m_handle, data, len, nullptr);
}

int ProtoHdlc::run_tx(void *data, int max_len)
{
    return hdlc_get_tx_data( m_handle, data, max_len );
}

void ProtoHdlc::disableCrc()
{
    m_crc = HDLC_CRC_OFF;
}

void ProtoHdlc::enableCrc(hdlc_crc_t crc)
{
    m_crc = crc;
}

bool ProtoHdlc::enableCheckSum()
{
    m_crc = HDLC_CRC_8;
    return true;
}

bool ProtoHdlc::enableCrc16()
{
    m_crc = HDLC_CRC_16;
    return true;
}

bool ProtoHdlc::enableCrc32()
{
    m_crc = HDLC_CRC_32;
    return true;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

}  // namespace Tiny

