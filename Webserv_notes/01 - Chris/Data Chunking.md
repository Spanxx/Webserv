📆 2025-05-31 🕓 11:17
Tags: 

---

# What is a Data Chunk?

Data chunks are often used in databases to store information about specific topics or categories. For example, if you have a database that contains information about different types of cars, each car would be considered its own data chunk.

# Chunking in Programming

Chunking is the process of breaking down large amounts of data into smaller, more manageable pieces. It’s an important concept in programming because it allows programmers to work with larger sets of information without becoming overwhelmed by the volume. This allows for faster retrieval and analysis when needed while reducing the amount of [memory](https://www.webopedia.com/definitions/memory/) required to store it all at once.

In addition, it can help reduce network traffic during transmission by sending only relevant portions rather than entire files or databases at once.

## [RFC2960](https://www.rfc-editor.org/rfc/rfc2960) Stream Control Transmission Protocol ([SCTP](https://www.webopedia.com/definitions/sctp/))

- offers recognized error-free non-duplicate transfer of messages
- detection of data loss, data corruption and duplication of data
	- achieved by using checksums and sequence numbers
- selective retransmission mechanism is applied to correct loss or curruption of data

```ad-info
**Checksum**
A simple [error-detection](https://www.webopedia.com/definitions/error-detection/) scheme in which each transmitted message is accompanied by a numerical value based on the number of set [bits](https://www.webopedia.com/definitions/bit/) in the message. The receiving station then applies the same formula to the message and checks to make sure the accompanying numerical value is the same. If not, the receiver can assume that the message has been garbled.
```

- SCTP connections are established after a 4-way handshake between two endpoints
	- ususally client and server

1. Server gets clients association setup request
2. Server returns an acknowledgement of the setup containing a data structure called cookie
	1. the cookie is protected by a secure message authentication code, and does not change state
3. when client returns the unchanged cookie
4. only then the server allocate ressources and establish a new association