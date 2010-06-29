/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2010, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <ostream>

#include <ip/bdd/operators.hh>
#include <ip/bdd/operators-int.hh>


template class ip::bdd::binary_operator<std::logical_and<bool> >;
template class ip::bdd::binary_operator<std::logical_or<bool> >;
