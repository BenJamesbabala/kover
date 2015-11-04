import h5py as h
import logging
import numpy as np

from math import ceil

def _class_to_string(instance):
    """
    Returns a string representation of the public attributes of a class.
    Parameters:
    -----------
    instance: object
        An instance of any class.
    Returns:
    --------
    string_rep: string
        A string representation of the class and its public attributes.
    Notes:
    -----
    Private attributes must be marked with a leading underscore.
    """
    return instance.__class__.__name__ + "(" + ",".join(
        [str(k) + "=" + str(v) for k, v in instance.__dict__.iteritems() if str(k[0]) != "_"]) + ")"

def _duplicate_last_element(l, length):
    """
    Duplicates the last element of a list until a given length is reached. (In-place)
    """
    l += [l[-1]] * (length - len(l))
    return l

def _hdf5_open_no_chunk_cache(filename, access_type=h.h5f.ACC_RDONLY):
    fid = h.h5f.open(filename, access_type)
    access_property_list = fid.get_access_plist()
    cache_properties = list(access_property_list.get_cache())
    fid.close()
    # Disable chunk caching
    cache_properties[2] = 0  # No chunk caching
    access_property_list.set_cache(*cache_properties)
    file_id = h.h5f.open(filename, access_type, fapl=access_property_list)
    return h.File(file_id)

def _minimum_uint_size(max_value):
    """
    Find the minimum size unsigned integer type that can store values of at most max_value
    """
    if max_value <= np.iinfo(np.uint8).max:
        return np.uint8
    elif max_value <= np.iinfo(np.uint16).max:
        return np.uint16
    elif max_value <= np.iinfo(np.uint32).max:
        return np.uint32
    elif max_value <= np.iinfo(np.uint64).max:
        return np.uint64
    else:
        return np.uint128

def _pack_binary_bytes_to_ints(a, pack_size):
    """
    Packs binary values stored in bytes into ints
    """
    if pack_size == 64:
        type = np.uint64
    elif pack_size == 32:
        type = np.uint32
    else:
        raise ValueError("Supported data types are 32-bit and 64-bit integers.")

    b = np.zeros((int(ceil(1.0 * a.shape[0] / pack_size)), a.shape[1]), dtype=type)
    packed_rows = 0
    packing_row = 0
    for i in xrange(a.shape[0]):
        if packed_rows == pack_size:
            packed_rows = 0
            packing_row += 1
        tmp = np.asarray(a[i], dtype=type)
        tmp = np.left_shift(tmp, type(pack_size - packed_rows - 1))
        np.bitwise_or(b[packing_row], tmp, out=b[packing_row])
        packed_rows += 1

    return b

def _unpack_binary_bytes_from_ints(a):
    """
    Unpacks binary values stored in bytes into ints
    """
    type = a.dtype

    if type == np.uint32:
        pack_size = 32
    elif type == np.uint64:
        pack_size = 64
    else:
        raise ValueError("Supported data types are 32-bit and 64-bit integers.")

    unpacked_n_rows = a.shape[0] * pack_size
    unpacked_n_columns = a.shape[1] if len(a.shape) > 1 else 1
    b = np.zeros((unpacked_n_rows, a.shape[1]) if len(a.shape) > 1 else unpacked_n_rows, dtype=np.uint8)

    packed_rows = 0
    packing_row = 0
    for i in xrange(b.shape[0]):
        if packed_rows == pack_size:
            packed_rows = 0
            packing_row += 1
        tmp = np.left_shift(np.ones(unpacked_n_columns, dtype=type), pack_size - (i - pack_size * packing_row)-1)
        np.bitwise_and(a[packing_row], tmp, tmp)
        b[i] = tmp > 0
        packed_rows += 1

    return b