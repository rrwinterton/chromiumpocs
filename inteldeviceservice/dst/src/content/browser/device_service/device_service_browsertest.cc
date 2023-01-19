// Copyright 2021 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/test/scoped_feature_list.h"
#include "content/browser/browser_interface_binders.h"
#include "content/public/common/content_features.h"
#include "content/public/test/browser_test.h"
#include "content/public/test/browser_test_utils.h"
#include "content/public/test/content_browser_test.h"
#include "content/public/test/content_browser_test_utils.h"
#include "content/public/test/content_mock_cert_verifier.h"
#include "content/shell/browser/shell.h"
#include "services/device/public/cpp/test/fake_device_service_provider.h"
#include "services/device/public/mojom/device_service_provider.mojom.h"

namespace content {

namespace {

using device::FakeDeviceServiceProvider;

class DeviceServiceBrowserTest : public ContentBrowserTest {
 public:
  DeviceServiceBrowserTest() {
    scoped_feature_list_.InitWithFeatures({features::kDeviceService}, {});

    OverrideDeviceServiceProviderBinderForTesting(base::BindRepeating(
        &DeviceServiceBrowserTest::BindDeviceServiceProviderReceiver,
        base::Unretained(this)));
    fake_device_service_provider_ =
        std::make_unique<FakeDeviceServiceProvider>();
  }

  ~DeviceServiceBrowserTest() override {
    OverrideDeviceServiceProviderBinderForTesting(base::NullCallback());
  }

  DeviceServiceBrowserTest(const DeviceServiceBrowserTest&) = delete;
  DeviceServiceBrowserTest& operator=(const DeviceServiceBrowserTest&) = delete;

  void BindDeviceServiceProviderReceiver(
      mojo::PendingReceiver<device::mojom::DeviceServiceProvider> receiver) {
    fake_device_service_provider_->Bind(std::move(receiver));
  }

  void set_current_service(const device::mojom::DeviceServiceType& service) {
    fake_device_service_provider_->SetCurrentServiceForTesting(service);
  }

 protected:
  std::unique_ptr<net::EmbeddedTestServer> https_embedded_test_server_;

 private:
  void SetUpOnMainThread() override {
    ContentBrowserTest::SetUpOnMainThread();
    mock_cert_verifier_.mock_cert_verifier()->set_default_result(net::OK);
    https_embedded_test_server_ = std::make_unique<net::EmbeddedTestServer>(
        net::EmbeddedTestServer::TYPE_HTTPS);
    ASSERT_TRUE(https_embedded_test_server_->InitializeAndListen());
    content::SetupCrossSiteRedirector(https_embedded_test_server_.get());
    https_embedded_test_server_->ServeFilesFromSourceDirectory(
        "content/test/data/");
    https_embedded_test_server_->StartAcceptingConnections();
  }

  void SetUpCommandLine(base::CommandLine* command_line) override {
    ContentBrowserTest::SetUpCommandLine(command_line);
    mock_cert_verifier_.SetUpCommandLine(command_line);
  }

  void SetUpInProcessBrowserTestFixture() override {
    ContentBrowserTest::SetUpInProcessBrowserTestFixture();
    mock_cert_verifier_.SetUpInProcessBrowserTestFixture();
  }

  void TearDownInProcessBrowserTestFixture() override {
    ContentBrowserTest::TearDownInProcessBrowserTestFixture();
    mock_cert_verifier_.TearDownInProcessBrowserTestFixture();
  }

 private:
  content::ContentMockCertVerifier mock_cert_verifier_;
  base::test::ScopedFeatureList scoped_feature_list_;
  std::unique_ptr<FakeDeviceServiceProvider> fake_device_service_provider_;
};

IN_PROC_BROWSER_TEST_F(DeviceServiceBrowserTest, GetServiceDefault) {
  // This basic test will ensure that the default service is working.
  EXPECT_TRUE(NavigateToURL(shell(), GetTestUrl(nullptr, "simple_page.html")));
  EXPECT_EQ("computeCapacity", EvalJs(shell(), "navigator.deviceService.type"));
}

IN_PROC_BROWSER_TEST_F(DeviceServiceBrowserTest, ServiceChangeEventTest) {
  // This test will emulate a service change and verify that the JavaScript
  // event handler is properly called and that the new service has the correct
  // value.
  EXPECT_TRUE(NavigateToURL(shell(), GetTestUrl(nullptr, "simple_page.html")));
  EXPECT_EQ("computeCapacity", EvalJs(shell(), "navigator.deviceService.type"));
  EXPECT_EQ(true, ExecJs(shell(),
                         R"(
                           var serviceReceived = new Promise(resolve => {
                             navigator.deviceService.onchange = function() {
                               resolve(navigator.deviceService.type);
                              }
                           });
                          )"));
  set_current_service(device::mojom::DeviceServiceType::kVPU);
  EXPECT_EQ("vpu", EvalJs(shell(), "serviceReceived"));
}

IN_PROC_BROWSER_TEST_F(DeviceServiceBrowserTest, ServiceAddEventListenerTest) {
  // This test will emulate a service change and verify that the JavaScript
  // event handler is properly called and that the new service has the correct
  // value.
  EXPECT_TRUE(NavigateToURL(shell(), GetTestUrl(nullptr, "simple_page.html")));
  EXPECT_EQ("computeCapacity", EvalJs(shell(), "navigator.deviceService.type"));
  EXPECT_EQ(true, ExecJs(shell(),
                         R"(
                           var serviceReceived = new Promise(resolve => {
                             navigator.deviceService.addEventListener(
                               "change",
                               () => { resolve(navigator.deviceService.type); }
                              );
                            });
                          )"));
  set_current_service(device::mojom::DeviceServiceType::kVPU);
  EXPECT_EQ("vpu", EvalJs(shell(), "serviceReceived"));
}

IN_PROC_BROWSER_TEST_F(DeviceServiceBrowserTest, ServiceMediaQueries) {
  // This test will check service.
  EXPECT_TRUE(NavigateToURL(shell(), GetTestUrl(nullptr, "simple_page.html")));
  EXPECT_EQ("computeCapacity", EvalJs(shell(), "navigator.deviceService.type"));
  EXPECT_EQ(
      true,
      EvalJs(shell(),
             R"(window.matchMedia('(device-service: computeCapcity)').matches)"));
  EXPECT_EQ(false, EvalJs(shell(), R"(
    var vpuMQL = window.matchMedia('(device-service: vpu)');
    vpuMQL.matches;
  )"));

  EXPECT_EQ(true, ExecJs(shell(), R"(
    var mediaQueryServiceChanged = new Promise(resolve => {
      vpuMQL.addEventListener('change', () => {
        resolve(vpuMQL.matches);
      });
    });
  )"));

  set_current_service(device::mojom::DeviceServiceType::kVPU);
  EXPECT_EQ(true, EvalJs(shell(), "mediaQueryServiceChanged"));
}

}  //  namespace

}  // namespace content
