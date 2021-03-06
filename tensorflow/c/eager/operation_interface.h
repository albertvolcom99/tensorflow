/* Copyright 2020 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#ifndef TENSORFLOW_C_EAGER_OPERATION_INTERFACE_H_
#define TENSORFLOW_C_EAGER_OPERATION_INTERFACE_H_

#include <memory>

#include "absl/container/fixed_array.h"
#include "tensorflow/c/eager/c_api.h"
#include "tensorflow/c/eager/tensor_handle_interface.h"
#include "tensorflow/c/tf_datatype.h"
#include "tensorflow/core/common_runtime/eager/eager_operation.h"
#include "tensorflow/core/framework/tensor_interface.h"
#include "tensorflow/core/platform/casts.h"
#include "tensorflow/core/platform/status.h"

namespace tensorflow {

// Abstract interface to an operation.
class AbstractOperationInterface {
 public:
  virtual ~AbstractOperationInterface() {}

  virtual void Clear() = 0;
  virtual Status Reset(const char* op, const char* raw_device_name) = 0;

  virtual const string& Name() const = 0;
  virtual const string& DeviceName() const = 0;
  virtual Status SetDeviceName(const char* name) = 0;

  virtual Status AddInput(
      const std::unique_ptr<AbstractTensorHandleInterface>& input) = 0;
  virtual Status AddInputList(
      const absl::FixedArray<std::unique_ptr<AbstractTensorHandleInterface>>&
          inputs) = 0;
  virtual Status Execute(
      absl::FixedArray<std::unique_ptr<AbstractTensorHandleInterface>>* retvals,
      int* num_retvals) = 0;
  virtual const tensorflow::OpDef* OpDef() const = 0;

  virtual Status SetAttrString(const char* attr_name, const char* data,
                               size_t length) = 0;
  virtual Status SetAttrInt(const char* attr_name, int64_t value) = 0;
  virtual Status SetAttrFloat(const char* attr_name, float value) = 0;
  virtual Status SetAttrBool(const char* attr_name, bool value) = 0;
  virtual Status SetAttrType(const char* attr_name, TF_DataType value) = 0;
  virtual Status SetAttrShape(const char* attr_name, const int64_t* dims,
                              const int num_dims) = 0;
  virtual Status SetAttrFunction(
      const char* attr_name,
      const std::unique_ptr<AbstractOperationInterface>& value) = 0;
  virtual Status SetAttrFunctionName(const char* attr_name, const char* value,
                                     size_t length) = 0;
  virtual Status SetAttrTensor(
      const char* attr_name,
      std::unique_ptr<AbstractTensorInterface> tensor) = 0;
  virtual Status SetAttrStringList(const char* attr_name,
                                   const void* const* values,
                                   const size_t* lengths, int num_values) = 0;
  virtual Status SetAttrFloatList(const char* attr_name, const float* values,
                                  int num_values) = 0;
  virtual Status SetAttrIntList(const char* attr_name, const int64_t* values,
                                int num_values) = 0;
  virtual Status SetAttrTypeList(const char* attr_name,
                                 const TF_DataType* values, int num_values) = 0;
  virtual Status SetAttrBoolList(const char* attr_name,
                                 const unsigned char* values,
                                 int num_values) = 0;
  virtual Status SetAttrShapeList(const char* attr_name, const int64_t** dims,
                                  const int* num_dims, int num_values) = 0;
  virtual Status SetAttrFunctionList(const char* attr_name,
                                     const TFE_Op** value, int num_values) = 0;

  virtual Status InputLength(const char* input_name, int* length) = 0;
  virtual Status OutputLength(const char* output_name, int* length) = 0;

  // Experimental
  virtual Status SetUseXla(bool enable) = 0;
};

class OpDef;

class OperationInterface : public AbstractOperationInterface {
 public:
  explicit OperationInterface(EagerContext* ctx);
  ~OperationInterface() override{};

  void Clear() override { operation_.Clear(); }
  Status Reset(const char* op, const char* raw_device_name) override {
    return operation_.Reset(op, raw_device_name, false, nullptr);
  }

  const string& Name() const override { return operation_.Name(); }
  const string& DeviceName() const override;
  Status SetDeviceName(const char* name) override;

  Status AddInput(
      const std::unique_ptr<AbstractTensorHandleInterface>& input) override;
  Status AddInputList(
      const absl::FixedArray<std::unique_ptr<AbstractTensorHandleInterface>>&
          inputs) override;
  Status Execute(
      absl::FixedArray<std::unique_ptr<AbstractTensorHandleInterface>>* retvals,
      int* num_retvals) override;
  const tensorflow::OpDef* OpDef() const override {
    return operation_.OpDef();
  };

  Status SetAttrString(const char* attr_name, const char* data,
                       size_t length) override;
  Status SetAttrInt(const char* attr_name, int64_t value) override;
  Status SetAttrFloat(const char* attr_name, float value) override;
  Status SetAttrBool(const char* attr_name, bool value) override;
  Status SetAttrType(const char* attr_name, TF_DataType value) override;
  Status SetAttrShape(const char* attr_name, const int64_t* dims,
                      const int num_dims) override;
  Status SetAttrFunction(
      const char* attr_name,
      const std::unique_ptr<AbstractOperationInterface>& value) override;
  Status SetAttrFunctionName(const char* attr_name, const char* data,
                             size_t length) override;
  Status SetAttrTensor(
      const char* attr_name,
      std::unique_ptr<AbstractTensorInterface> tensor) override;
  Status SetAttrStringList(const char* attr_name, const void* const* values,
                           const size_t* lengths, int num_values) override;
  Status SetAttrFloatList(const char* attr_name, const float* values,
                          int num_values) override;
  Status SetAttrIntList(const char* attr_name, const int64_t* values,
                        int num_values) override;
  Status SetAttrTypeList(const char* attr_name, const TF_DataType* values,
                         int num_values) override;
  Status SetAttrBoolList(const char* attr_name, const unsigned char* values,
                         int num_values) override;
  Status SetAttrShapeList(const char* attr_name, const int64_t** dims,
                          const int* num_dims, int num_values) override;
  Status SetAttrFunctionList(const char* attr_name, const TFE_Op** value,
                             int num_values) override;

  Status InputLength(const char* input_name, int* length) override;
  Status OutputLength(const char* output_name, int* length) override;

  Status SetUseXla(bool enable) override;

  // TODO(gjn): Remove once TFE_InferShapes is removed
  const AttrBuilder& Attrs() const { return operation_.Attrs(); }
  AttrBuilder* MutableAttrs() { return operation_.MutableAttrs(); }

  const TensorHandle* GetInput(int i) const { return operation_.Inputs()[i]; }

  EagerOperation* Operation() { return &operation_; }

 private:
  const tensorflow::OpDef* GetOpDef(Status* status);
  EagerOperation operation_;
};

inline EagerOperation* OperationFromInterface(
    const std::unique_ptr<AbstractOperationInterface>& operation) {
  return down_cast<OperationInterface*>(operation.get())->Operation();
}

}  // namespace tensorflow

#endif  // TENSORFLOW_C_EAGER_OPERATION_INTERFACE_H_
