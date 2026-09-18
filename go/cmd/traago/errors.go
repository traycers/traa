package main

// cliError is the process-level error envelope, mirroring the C++ runner's
// CliError (cxx/traapp/sources/main.cpp): code is one of the five fixed
// snake_case error codes from
// .scratch/array-trees/issues/09-cli-schema-details.md
// (invalid_tree_json, invalid_case_json, unknown_op, args_mismatch,
// file_not_found).
type cliError struct {
	Code    string
	Message string
}

func (e *cliError) Error() string {
	return e.Message
}

func newCliError(code, message string) *cliError {
	return &cliError{Code: code, Message: message}
}
