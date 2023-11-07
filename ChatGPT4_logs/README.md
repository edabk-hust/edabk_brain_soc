# Chats used to make hardware - software co-design for SNN

In this directory, are the interactions we've had with ChatGPT (GPT-4). These conversations were instrumental in building a RISCV-based design for a Brain-inspired Computing Architecture: Spike Neural Network (SNN).

## Multiple Chat Sessions:

The design has been developed based on multiple chat sessions rather than just one. This approach has the following advantages:

- It allows for the creation of multiple interacting chat segments, similar to how humans can interact and send feedback to each other during a work process.
- It allows for modularization, breaking down a large project and design steps into smaller modules, each with more specific requirements. This is also convenient for storing and searching information.
- Limit the occurrence of loops of incorrect responses, when ChatGPT provides a series of similar but incorrect answers.

## The Contributions of ChatGPT:

We utilized ChatGPT to generate numerous programming code segments as well as idea support:
- RTL design: including design small modules, integration, and wrappers
- RTL review and simple testbench creation
- C code for control
- Environment creation for testing. 
- Create an optimal memory map

These code segments would then be compiled together.

> However, each response from the AI still needs to be verified (they are just like "apprentices" learning the job). Thus, although the code segments are written by AI 100%, concurrently during the interaction with AI, we employed some tools (e.g., Questasim, ...) to run simulations to detect errors and provide feedback to the AI for code correction.

# ChatGPT 4.0 log files:
## RTL design chatlog
### Neuron Core's module generation
- SNN Memory modules (synapse_matrix, neuron_parameters, neuron_spike_out):
https://chat.openai.com/share/27bdc9b1-b97e-49ca-9681-5772234f8b4d
- SNN Neuron Block
https://chat.openai.com/share/2d025ea6-19ef-4a54-a3f9-a9f78141aa0d
- SNN Address Decoder
https://chat.openai.com/share/7ea21ee0-e090-4f7a-a4b8-72e1d9567b48

### Neuron Core (top_module)
- First attempt: Integration a 256x256 core
https://chat.openai.com/share/a7a74e23-559f-40cc-9a38-c9c1a15d608b
- Second attempt: (reduce number of neuron from 256 to 32):
https://chat.openai.com/share/e6262b11-6252-4525-9ab7-62ebd6356d08

### Simple testbench creation
- https://chat.openai.com/share/e84d0863-93e9-4de9-9ee3-b0bb898b7a7d
### user_project_wrapper
- https://chat.openai.com/share/3ffaa58f-c77f-4b4d-9b83-35487e76d84a

## Code C chatlog
- https://chat.openai.com/share/24c2843b-0db7-40dc-91f3-b21feaf3ff38

*By EDABK lab's members, November 2023*