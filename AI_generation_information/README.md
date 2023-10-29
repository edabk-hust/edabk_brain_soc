# Chats used to make a CNN

Within this directory, you'll find the interactions I've had with ChatGPT (GPT-4). These conversations were instrumental in building a CNN from scratch, with only minimal human intervention.

# A couple of important points to consider:

## Multiple Chat Sessions: 

The processors weren't developed in a single conversation. As the workflow illustrates, the process transitioned from Python to RTL code, encompassing four main sessions: CNN with TensorFlow, Python, C++, and HLS. Therefore, numerous chat interactions were needed to ensure a successful code implementation.

**This approach was adopted for a few reasons:**

 * There's a text limitation for both input and output when using ChatGPT.
 * It ensures the generation of more accurate responses.
 * It allows breaking down a broad problem into more specific, manageable parts.
 * This methodology was especially useful when traversing the different abstraction levels of design, from Python to HLS. The AI's precision in providing programming code was particularly beneficial.

## Testing & Verification: 

After executing each step, a set of tests compared the current results with those of the previous stage. This practice minimized iterations and quickly addressed any discrepancies.

## Loop of Incorrect Responses: 

There were instances when ChatGPT would provide a series of similar, yet incorrect answers. Whenever this happened, it signaled the need to initiate a new chat session.

# The Role of ChatGPT:

The AI supplied numerous sections  of code. These were then integrated into the main function step by step. To draw an analogy, imagine me (or yourself) as a project manager, and ChatGPT as a team of intern engineers creating varied codes on demand. However, it's crucial to remember that each intern engineer's response (AI' response) can potentially be incomplete or contain errors. This underscores the importance of diligent oversight and verification at every stage of development.

By Emilio Baungarten, September 2023

