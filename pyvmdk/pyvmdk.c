/*
 * Python bindings module for libvmdk (pyvmdk)
 *
 * Copyright (c) 2009-2014, Joachim Metz <joachim.metz@gmail.com>
 *
 * Refer to AUTHORS for acknowledgements.
 *
 * This software is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this software.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <common.h>
#include <types.h>

#if defined( HAVE_STDLIB_H ) || defined( HAVE_WINAPI )
#include <stdlib.h>
#endif

#include "pyvmdk.h"
#include "pyvmdk_extent_descriptor.h"
#include "pyvmdk_extent_descriptors.h"
#include "pyvmdk_error.h"
#include "pyvmdk_libcerror.h"
#include "pyvmdk_libcstring.h"
#include "pyvmdk_libvmdk.h"
#include "pyvmdk_file_object_io_handle.h"
#include "pyvmdk_handle.h"
#include "pyvmdk_python.h"
#include "pyvmdk_unused.h"

#if !defined( LIBVMDK_HAVE_BFIO )
LIBVMDK_EXTERN \
int libvmdk_check_file_signature_file_io_handle(
     libbfio_handle_t *file_io_handle,
     libvmdk_error_t **error );
#endif

/* The pyvmdk module methods
 */
PyMethodDef pyvmdk_module_methods[] = {
	{ "get_version",
	  (PyCFunction) pyvmdk_get_version,
	  METH_NOARGS,
	  "get_version() -> String\n"
	  "\n"
	  "Retrieves the version." },

	{ "check_file_signature",
	  (PyCFunction) pyvmdk_check_file_signature,
	  METH_VARARGS | METH_KEYWORDS,
	  "check_file_signature(filename) -> Boolean\n"
	  "\n"
	  "Checks if a file has a VMware Virtual Disk (VMDK) file signature." },

	{ "check_file_signature_file_object",
	  (PyCFunction) pyvmdk_check_file_signature_file_object,
	  METH_VARARGS | METH_KEYWORDS,
	  "check_file_signature(file_object) -> Boolean\n"
	  "\n"
	  "Checks if a file has a VMware Virtual Disk (VMDK) file signature using a file-like object." },

	{ "open",
	  (PyCFunction) pyvmdk_handle_new_open,
	  METH_VARARGS | METH_KEYWORDS,
	  "open(filename, mode='r') -> Object\n"
	  "\n"
	  "Opens a VMDK image handle using the descriptor file." },

	{ "open_file_object",
	  (PyCFunction) pyvmdk_handle_new_open_file_object,
	  METH_VARARGS | METH_KEYWORDS,
	  "open(file_object, mode='r') -> Object\n"
	  "\n"
	  "Opens a VMDK image handle using a file-like object of the descriptor file." },

	/* Sentinel */
	{ NULL, NULL, 0, NULL }
};

/* Retrieves the pyvmdk/libvmdk version
 * Returns a Python object if successful or NULL on error
 */
PyObject *pyvmdk_get_version(
           PyObject *self PYVMDK_ATTRIBUTE_UNUSED,
           PyObject *arguments PYVMDK_ATTRIBUTE_UNUSED )
{
	const char *errors           = NULL;
	const char *version_string   = NULL;
	size_t version_string_length = 0;

	PYVMDK_UNREFERENCED_PARAMETER( self )
	PYVMDK_UNREFERENCED_PARAMETER( arguments )

	Py_BEGIN_ALLOW_THREADS

	version_string = libvmdk_get_version();

	Py_END_ALLOW_THREADS

	version_string_length = libcstring_narrow_string_length(
	                         version_string );

	/* Pass the string length to PyUnicode_DecodeUTF8
	 * otherwise it makes the end of string character is part
	 * of the string
	 */
	return( PyUnicode_DecodeUTF8(
	         version_string,
	         (Py_ssize_t) version_string_length,
	         errors ) );
}

/* Checks if the file has a VMware Virtual Disk (VMDK) file signature
 * Returns a Python object if successful or NULL on error
 */
PyObject *pyvmdk_check_file_signature(
           PyObject *self PYVMDK_ATTRIBUTE_UNUSED,
           PyObject *arguments,
           PyObject *keywords )
{
	libcerror_error_t *error    = NULL;
	static char *function       = "pyvmdk_check_file_signature";
	static char *keyword_list[] = { "filename", NULL };
	const char *filename        = NULL;
	int result                  = 0;

	PYVMDK_UNREFERENCED_PARAMETER( self )

	if( PyArg_ParseTupleAndKeywords(
	     arguments,
	     keywords,
	     "|s",
	     keyword_list,
	     &filename ) == 0 )
	{
		return( NULL );
	}
	Py_BEGIN_ALLOW_THREADS

	result = libvmdk_check_file_signature(
	          filename,
	          &error );

	Py_END_ALLOW_THREADS

	if( result == -1 )
	{
		pyvmdk_error_raise(
		 error,
		 PyExc_IOError,
		 "%s: unable to check file signature.",
		 function );

		libcerror_error_free(
		 &error );

		return( NULL );
	}
	if( result != 0 )
	{
		return( Py_True );
	}
	return( Py_False );
}

/* Checks if the file has a VMware Virtual Disk (VMDK) file signature using a file-like object
 * Returns a Python object if successful or NULL on error
 */
PyObject *pyvmdk_check_file_signature_file_object(
           PyObject *self PYVMDK_ATTRIBUTE_UNUSED,
           PyObject *arguments,
           PyObject *keywords )
{
	libcerror_error_t *error         = NULL;
	libbfio_handle_t *file_io_handle = NULL;
	PyObject *file_object            = NULL;
	static char *function            = "pyvmdk_check_file_signature_file_object";
	static char *keyword_list[]      = { "file_object", NULL };
	int result                       = 0;

	PYVMDK_UNREFERENCED_PARAMETER( self )

	if( PyArg_ParseTupleAndKeywords(
	     arguments,
	     keywords,
	     "|O",
	     keyword_list,
	     &file_object ) == 0 )
	{
		return( NULL );
	}
	if( pyvmdk_file_object_initialize(
	     &file_io_handle,
	     file_object,
	     &error ) != 1 )
	{
		pyvmdk_error_raise(
		 error,
		 PyExc_MemoryError,
		 "%s: unable to initialize file IO handle.",
		 function );

		libcerror_error_free(
		 &error );

		goto on_error;
	}
	Py_BEGIN_ALLOW_THREADS

	result = libvmdk_check_file_signature_file_io_handle(
	          file_io_handle,
	          &error );

	Py_END_ALLOW_THREADS

	if( result == -1 )
	{
		pyvmdk_error_raise(
		 error,
		 PyExc_IOError,
		 "%s: unable to check file signature.",
		 function );

		libcerror_error_free(
		 &error );

		goto on_error;
	}
	if( libbfio_handle_free(
	     &file_io_handle,
	     &error ) != 1 )
	{
		pyvmdk_error_raise(
		 error,
		 PyExc_MemoryError,
		 "%s: unable to free file IO handle.",
		 function );

		libcerror_error_free(
		 &error );

		goto on_error;
	}
	if( result != 0 )
	{
		return( Py_True );
	}
	return( Py_False );

on_error:
	if( file_io_handle != NULL )
	{
		libbfio_handle_free(
		 &file_io_handle,
		 NULL );
	}
	return( NULL );
}

/* Declarations for DLL import/export
 */
#ifndef PyMODINIT_FUNC
#define PyMODINIT_FUNC void
#endif

/* Initializes the pyvmdk module
 */
PyMODINIT_FUNC initpyvmdk(
                void )
{
	PyObject *module                             = NULL;
	PyTypeObject *extent_descriptor_type_object  = NULL;
	PyTypeObject *extent_descriptors_type_object = NULL;
	PyTypeObject *handle_type_object             = NULL;
	PyGILState_STATE gil_state                   = 0;

	/* Create the module
	 * This function must be called before grabbing the GIL
	 * otherwise the module will segfault on a version mismatch
	 */
	module = Py_InitModule3(
	          "pyvmdk",
	          pyvmdk_module_methods,
	          "Python libvmdk module (pyvmdk)." );

	PyEval_InitThreads();

	gil_state = PyGILState_Ensure();

	/* Setup the handle type object
	 */
	pyvmdk_handle_type_object.tp_new = PyType_GenericNew;

	if( PyType_Ready(
	     &pyvmdk_handle_type_object ) < 0 )
	{
		goto on_error;
	}
	Py_IncRef(
	 (PyObject *) &pyvmdk_handle_type_object );

	handle_type_object = &pyvmdk_handle_type_object;

	PyModule_AddObject(
	 module,
	 "handle",
	 (PyObject *) handle_type_object );

	/* Setup the extent descriptors type object
	 */
	pyvmdk_extent_descriptors_type_object.tp_new = PyType_GenericNew;

	if( PyType_Ready(
	     &pyvmdk_extent_descriptors_type_object ) < 0 )
	{
		goto on_error;
	}
	Py_IncRef(
	 (PyObject *) &pyvmdk_extent_descriptors_type_object );

	extent_descriptors_type_object = &pyvmdk_extent_descriptors_type_object;

	PyModule_AddObject(
	 module,
	 "_extent_descriptors",
	 (PyObject *) extent_descriptors_type_object );

	/* Setup the extent descriptor type object
	 */
	pyvmdk_extent_descriptor_type_object.tp_new = PyType_GenericNew;

	if( PyType_Ready(
	     &pyvmdk_extent_descriptor_type_object ) < 0 )
	{
		goto on_error;
	}
	Py_IncRef(
	 (PyObject *) &pyvmdk_extent_descriptor_type_object );

	extent_descriptor_type_object = &pyvmdk_extent_descriptor_type_object;

	PyModule_AddObject(
	 module,
	 "extent_descriptor",
	 (PyObject *) extent_descriptor_type_object );

on_error:
	PyGILState_Release(
	 gil_state );
}

