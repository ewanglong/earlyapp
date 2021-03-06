////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2018 Intel Corporation
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom
// the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
// OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
// OR OTHER DEALINGS IN THE SOFTWARE.
//
// SPDX-License-Identifier: MIT
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CBCEventDevice.hpp"
#include "CBCEvent.hpp"

namespace earlyapp
{
    /**
      @brief A virtual CBC Event device that can be used when no CAN BOX physical device available.
     */
    class VirtualCBCEventDevice: public CBCEventDevice
    {
    public:
        /**
           @brief Constructor.
           @param cbcDevice Path for CBC device node.
        */
        VirtualCBCEventDevice(const char* cbcDevice=nullptr);

        /**
           @brief Disable copy constructor.
        */
        VirtualCBCEventDevice(const VirtualCBCEventDevice&) = delete;

        /**
           @brief Destructor.
        */
        virtual ~VirtualCBCEventDevice(void);

        /**
           @brief Read CBC events from the file.
           @return CBC events to the virtual device file.
        */
        std::shared_ptr<CBCEvent> readEvent(void);

        /**
           @brief Disable assign operator.
        */
        VirtualCBCEventDevice& operator=(const VirtualCBCEventDevice&) = delete;

    private:
        char* m_pFileName = nullptr;
        int m_fdCBCDev = -1;
    };
} // namespace


