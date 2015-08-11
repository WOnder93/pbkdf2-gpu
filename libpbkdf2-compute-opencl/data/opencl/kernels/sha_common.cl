/*
 * Copyright (C) 2015, Ondrej Mosnacek <omosnacek@gmail.com>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation: either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SHA_COMMON_CL
#define SHA_COMMON_CL

#ifdef cl_nv_pragma_unroll
#define NVIDIA
#endif /* cl_nv_pragma_unroll */

/* These functions are used in various SHA versions,
   their optimized versions are provided here. */
#ifndef NVIDIA
#define SHA_F0(x,y,z) bitselect(z, y, x)
#else
#define SHA_F0(x,y,z) (z ^ (x & (y ^ z)))
#endif /* NVIDIA */

#define SHA_F1(x,y,z) bitselect(y, x, y ^ z)

#endif // SHA_COMMON_CL
