Intel Platform Framework Core SDK v0.7 Release Notes

**************** PREVIEW RELEASE ONLY; NOT FOR GENERAL RELEASE *****************
**          Contact Intel representatives if you have any questions           **
********************************************************************************

Contents:
    1.0 Disclaimers
    2.0 Overview
    3.0 Supported Operating Systems
    4.0 Supported Platforms
    5.0 Prerequisites
    6.0 Installation
    7.0 Sample Client Application

1.0 Disclaimers

    This document contains information on products in the design phase of
    development. The information here is subject to change without notice. Do
    not finalize a design with this information.

    The products described may contain design defects or errors known as errata
    which may cause the product to deviate from published specifications.
    Current characterized errata are available on request.

    No license (express or implied, by estoppel or otherwise) to any
    intellectual property rights is granted by this document.
 
    Intel technologies may require enabled hardware, specific software, or
    services activation. Check with your system manufacturer or retailer. 

    This software and the related documents are Intel copyrighted materials, and
    your use of them is governed by the express license under which they were
    provided to you. Unless the License provides otherwise, you may not use,
    modify, copy, publish, distribute, disclose or transmit this software or the
    related documents without Intel's prior written permission.

    This software and the related documents are provided as is, with no express
    or implied warranties, other than those that are expressly stated in the
    License.

    Intel, the Intel logo, and other Intel marks are trademarks of Intel
    Corporation or its subsidiaries. Other names and brands may be claimed as
    the property of others. 

    Copyright (c) 2013-2022 Intel Corporation All Rights Reserved All rights reserved.

2.0 Overview

    This folder contains the necessary headers, libraries, and project
    components necessary to build the IPF Core SDK Sample Application, which can
    be used to connect to a system running a compatible version of Intel(R)
    Platform Framework (IPF) support and collect realtime data using the IPF
    Core API.

    Refer to the IPF Core API User Guide for details on SDK contents and API
    specifications.  The instructions provided herein only contain the
    information necessary to install the SDK and build and run a sample client
    application.

3.0 Supported Operating Systems

    Refer to the DTT installation package release notes

4.0 Supported Platforms

    Refer to the DTT installation package release notes

5.0 Prerequisites

5.1 Drivers

    DTT with Intel Platform Framework (IPF) support must be installed on the
    test platform.  The installed drivers must be compatible the IPF Core API
    version provided by this SDK.  
    
    Refer to the DTT installation package release notes or contact your support
    representative to determine the IPF Core API supported.

5.2 Visual Studio 2019

    Visual Studio 2019 with the following required components:
      Workloads:
         Desktop development with C++
      Individual Components:
         MSVC v142 - VS 2019 C++ x64/x86 build tools (v14.24) [or higher]
         MSVC v142 - VS 2019 C++ x64/x86 Spectre-mitigated libs (v14.24) [or higher]
         Windows 10 SDK (10.0.18362.0)

    NOTE:  The instructions contained herein are based on use of a Visual
    Studio solution (SLN) to build a sample IPF client. 
    
6.0 Installation

    Unzip this ZIP file into any folder, such as C:\Intel\IPF_CORE_SDK.

7.0 Sample Client Application

7.1 Build Instructions

    Open IPF\Samples\Windows\samples.sln in Visual Studio 2019.

    Select Release or Debug x64 Build and select Build Solution from the Build
    menu.

    sampleapp1.exe will be in the IPF\Samples\Windows\x64\Release or Debug folder
      
7.2 Sample Code Overview

    sampleapp1.c is the main module and includes all necessary headers and 
    links with either the Release or Debug version of ipfcorelib.lib and builds a
    Windows UAP Console Application.
    
    ipfcoresdk.dll is experimental and not supported at this time.

7.3 Running the Sample

    Copy sampleapp1.exe to any folder on the test platform.
    
    Open a Windows Command Prompt (CMD.EXE) and run sampleapp1.exe.

    You should see the client connect and then display the CPU temperature,
    CPU power, and Set & Display the PL1/PL2 values every second and exit
    after about 10 seconds.
    
    NOTE: Administrator privileges (Run As Administrator...) ARE required by
          default in this release for the sample app to establish a connection.
          You may use a non-Adminstrator account by changing PROGRAM_SERVERADDR
          in the sample code to use DEFAULT_SERVERADDR or by passing a command
          line parameter of "--srv:ipfsrv.public" to the sample app. Either
          of these will cause the sample app to connect as an unpriviledged user,
          allowing the app to still READ PL values, but not SET them.
