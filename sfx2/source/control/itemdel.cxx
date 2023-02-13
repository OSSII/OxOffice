/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

#include <sal/config.h>

#include <itemdel.hxx>
#include <svl/poolitem.hxx>
#include <vcl/idle.hxx>
#include <vcl/svapp.hxx>

#include <tools/debug.hxx>

class SfxItemDisruptor_Impl
{
public:
    static void DeleteItemOnIdle(std::unique_ptr<SfxPoolItem> pItem)
    {
        pItem->SetKind(SfxItemKind::DeleteOnIdle);
        Application::PostUserEvent(LINK(nullptr, SfxItemDisruptor_Impl, Delete), pItem.release());
        // coverity[leaked_storage] - pDisruptor takes care of its own destruction at idle time
    }

    DECL_STATIC_LINK(SfxItemDisruptor_Impl, Delete, void*, void);
};

IMPL_STATIC_LINK(SfxItemDisruptor_Impl, Delete, void*, p, void)
{
    SfxPoolItem* pItem = static_cast<SfxPoolItem*>(p);
    // reset RefCount (was set to SFX_ITEMS_SPECIAL before!)
    pItem->SetRefCount(0);
    delete pItem;
}

void DeleteItemOnIdle(std::unique_ptr<SfxPoolItem> pItem)
{
    DBG_ASSERT(0 == pItem->GetRefCount(), "deleting item in use");
    SfxItemDisruptor_Impl::DeleteItemOnIdle(std::move(pItem));
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
