# CNN Inference for MNIST Dataset in Verilog: Testbench README

## Introduction
This README provides details on the Verilog testbench created to validate the CNN inference module for the MNIST dataset. The CNN module is designed to identify handwritten digits (0-9) using a trained CNN model in a hardware implementation.

## Dependencies
Caravel Simulator: Icarus Verilog
MNIST Dataset: Sample test inputs from the MNIST dataset to validate the CNN's inference.
Reference Outputs: Expected CNN inference results for the provided test inputs.

## Testbench Structure

user_project_wrapper.v: This is the top-level Verilog module of the CNN model.
AI_by_AI_test_tb.v: The main testbench file that feeds input data to user_project_wrapper.v and captures the output.
Test Vectors: Set of input vector files (from image_0.txt to image_99.txt) and expected output vector files (test_labels.txt).

All the necessary files to execute the testbench can be found in the [AI_by_AI/verilog/dv/Ay_by_AI_test/](https://github.com/Baungarten-CINVESTAV/AI_by_AI/tree/main/verilog/dv/Ay_by_AI_test) directory.

## Some Testbench

<p align="center">
<img src="https://github.com/Baungarten-CINVESTAV/AI_by_AI/assets/101527680/a2b73c08-21ee-4da3-bd39-a43fc2a1c5f7">

<img src="https://github.com/Baungarten-CINVESTAV/AI_by_AI/assets/101527680/1f80e89a-347d-49cf-a1ca-f995c01ed5c6">

<img src="https://github.com/Baungarten-CINVESTAV/AI_by_AI/assets/101527680/da66f80d-f16b-4587-acca-09979a5cca68">

<img src="https://github.com/Baungarten-CINVESTAV/AI_by_AI/assets/101527680/5caf744c-d5ea-4ee6-a155-96f96dade413">

<img src="https://github.com/Baungarten-CINVESTAV/AI_by_AI/assets/101527680/02508198-9ae3-47b9-982e-d57a80bcdda9">
</p>



