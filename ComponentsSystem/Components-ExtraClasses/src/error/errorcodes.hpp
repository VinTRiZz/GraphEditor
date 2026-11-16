#pragma once

/**
 * @file Error codes used by Components system
 */

namespace ExtraClasses
{

/**
 * @brief The ErrorCode enum Error code of a component
 */
enum ErrorCode : int
{

Unknown = -1,
Ok = 0,

// Zero-codes - system, library, etc.
// (defined by OS, binary, I/O or other sources)
SystemUnknown = 1,
SystemInterrupted,
SystemFileError,


// Logical
// (invalid input, failed to proceed, etc.)
LogicalUnknown = 10'000,
LogicalNotImplemented,
LogicalInvalidValue,
LogicalKeyGenerationFailed,
LogicalEncryptionFailed,
LogicalDecryptionFailed,
LogicalHashFailed,
LogicalEncodingFailed,


// Interface
// (TCP/IP transport and lower layer errors)
InterfaceUnknown = 20'000,


// Protocol
// (OSI session and upper layer errors)
ProtocolUnknown = 30'000,


// Database
// (invalid DB name, user, value, query, etc.)
DatabaseUnknown = 40'000,
DatabaseInvalidDriver,
DatabaseInvalidDatabase,
DatabaseInvalidTable,
DatabaseConnectionCreateFailed,
DatabaseQueryExecutionFailed,


// User-defined error codes
UserCode = 100'000
};

}