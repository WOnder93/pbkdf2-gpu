/*
 * PBKDF2-HMAC-SHA1 OpenCL kernel
 *
 * Copyright (c) 2012, Sayantan Datta <std2048 at gmail dot com>
 * Copyright (c) 2015, Ondrej Mosnacek <omosnacek@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Based on software published under the following license:
 *
 *     This software is Copyright (c) 2012 Sayantan Datta <std2048 at gmail dot com>
 *     and it is hereby released to the general public under the following terms:
 *     Redistribution and use in source and binary forms, with or without modification, are permitted.
 *     Based on S3nf implementation http://openwall.info/wiki/john/MSCash2
 *     Modified to support salts upto 19 characters. Bug in orginal code allowed only upto 8 characters.
 */

#include "hash_sha1.cl"
#include "pbkdf2_generic.cl"
