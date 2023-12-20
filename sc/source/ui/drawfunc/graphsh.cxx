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

#include <sfx2/objface.hxx>
#include <vcl/EnumContext.hxx>
#include <sfx2/opengrf.hxx>
#include <svx/svdograf.hxx>
#include <svx/grfflt.hxx>
#include <svx/grafctrl.hxx>
#include <svx/compressgraphicdialog.hxx>
#include <svx/graphichelper.hxx>
#include <svx/svxids.hrc>

#include <graphsh.hxx>
#include <strings.hrc>
#include <viewdata.hxx>
#include <drawview.hxx>
#include <gridwin.hxx>
#include <scresid.hxx>
#include <svx/extedit.hxx>
#include <sfx2/request.hxx>
#include <svl/stritem.hxx>
#include <vcl/graphicfilter.hxx>
#include <tools/urlobj.hxx>
#include <comphelper/lok.hxx>

#define ShellClass_ScGraphicShell
#include <scslots.hxx>

SFX_IMPL_INTERFACE(ScGraphicShell, ScDrawShell)

void ScGraphicShell::InitInterface_Impl()
{
    GetStaticInterface()->RegisterObjectBar(SFX_OBJECTBAR_OBJECT,
                                            SfxVisibilityFlags::Standard | SfxVisibilityFlags::Server,
                                            ToolbarId::Graphic_Objectbar);

    GetStaticInterface()->RegisterPopupMenu("graphic");
}


ScGraphicShell::ScGraphicShell(ScViewData& rData) :
    ScDrawShell(rData)
{
    SetName("GraphicObject");
    SfxShell::SetContextName(vcl::EnumContext::GetContextName(vcl::EnumContext::Context::Graphic));
}

ScGraphicShell::~ScGraphicShell()
{
}

void ScGraphicShell::GetAttrState( SfxItemSet& rSet )
{
    ScDrawView* pView = GetViewData().GetScDrawView();

    if( pView )
        SvxGrafAttrHelper::GetGrafAttrState( rSet, *pView );
}

void ScGraphicShell::Execute( SfxRequest& rReq )
{
    ScDrawView* pView = GetViewData().GetScDrawView();

    if( pView )
    {
        SvxGrafAttrHelper::ExecuteGrafAttr( rReq, *pView );
        Invalidate();
    }
}

void ScGraphicShell::GetFilterState( SfxItemSet& rSet )
{
    ScDrawView* pView = GetViewData().GetScDrawView();
    const SdrMarkList& rMarkList = pView->GetMarkedObjectList();
    bool bEnable = false;

    if( rMarkList.GetMarkCount() == 1 )
    {
        SdrObject* pObj = rMarkList.GetMark( 0 )->GetMarkedSdrObj();

        if( auto pGraphicObj = dynamic_cast<SdrGrafObj*>( pObj) )
            if( pGraphicObj->GetGraphicType() == GraphicType::Bitmap )
            bEnable = true;
    }

    if( !bEnable )
        SvxGraphicFilter::DisableGraphicFilterSlots( rSet );
}

void ScGraphicShell::ExecuteFilter( const SfxRequest& rReq )
{
    ScDrawView* pView = GetViewData().GetScDrawView();
    const SdrMarkList& rMarkList = pView->GetMarkedObjectList();

    if( rMarkList.GetMarkCount() == 1 )
    {
        SdrObject* pObj = rMarkList.GetMark( 0 )->GetMarkedSdrObj();

        if( auto pGraphicObj = dynamic_cast<SdrGrafObj*>( pObj) )
            if( pGraphicObj->GetGraphicType() == GraphicType::Bitmap )
            {
                GraphicObject aFilterObj( pGraphicObj->GetGraphicObject() );

                if( SvxGraphicFilterResult::NONE ==
                    SvxGraphicFilter::ExecuteGrfFilterSlot( rReq, aFilterObj ) )
                {
                    SdrPageView* pPageView = pView->GetSdrPageView();

                    if( pPageView )
                    {
                        rtl::Reference<SdrGrafObj> pFilteredObj = SdrObject::Clone(*pGraphicObj, pGraphicObj->getSdrModelFromSdrObject());
                        OUString    aStr = pView->GetDescriptionOfMarkedObjects() + " " + ScResId(SCSTR_UNDO_GRAFFILTER);
                        pView->BegUndo( aStr );
                        pFilteredObj->SetGraphicObject( aFilterObj );
                        pView->ReplaceObjectAtView( pObj, *pPageView, pFilteredObj.get() );
                        pView->EndUndo();
                    }
                }
            }
    }

    Invalidate();
}

void ScGraphicShell::GetExternalEditState( SfxItemSet& rSet )
{
    ScDrawView* pView = GetViewData().GetScDrawView();
    const SdrMarkList& rMarkList = pView->GetMarkedObjectList();
    bool bEnable = false;
    if( rMarkList.GetMarkCount() == 1 )
    {
        SdrObject* pObj = rMarkList.GetMark( 0 )->GetMarkedSdrObj();

        if( auto pGraphicObj = dynamic_cast<SdrGrafObj*>( pObj) )
            if( pGraphicObj->GetGraphicType() == GraphicType::Bitmap )
            bEnable = true;
    }

    if (GetObjectShell()->isExportLocked())
        bEnable = false;

    if( !bEnable )
        rSet.DisableItem( SID_EXTERNAL_EDIT );
}

void ScGraphicShell::ExecuteExternalEdit( SAL_UNUSED_PARAMETER SfxRequest& )
{
    ScDrawView* pView = GetViewData().GetScDrawView();
    const SdrMarkList& rMarkList = pView->GetMarkedObjectList();

    if( rMarkList.GetMarkCount() == 1 )
    {
        SdrObject* pObj = rMarkList.GetMark( 0 )->GetMarkedSdrObj();

        if( auto pGraphicObj = dynamic_cast<SdrGrafObj*>( pObj) )
            if( pGraphicObj->GetGraphicType() == GraphicType::Bitmap )
            {
                GraphicObject aGraphicObject( pGraphicObj->GetGraphicObject() );
                m_ExternalEdits.push_back( std::make_unique<SdrExternalToolEdit>(
                            pView, pGraphicObj));
                m_ExternalEdits.back()->Edit( &aGraphicObject );
            }
    }

    Invalidate();
}

void ScGraphicShell::GetCompressGraphicState( SfxItemSet& rSet )
{
    ScDrawView* pView = GetViewData().GetScDrawView();
    const SdrMarkList& rMarkList = pView->GetMarkedObjectList();
    bool bEnable = false;
    if( rMarkList.GetMarkCount() == 1 )
    {
        SdrObject* pObj = rMarkList.GetMark( 0 )->GetMarkedSdrObj();

        if( auto pGraphicObj = dynamic_cast<const SdrGrafObj*>( pObj) )
            if( pGraphicObj->GetGraphicType() == GraphicType::Bitmap )
                bEnable = true;
    }

    if( !bEnable )
        rSet.DisableItem( SID_COMPRESS_GRAPHIC );
}

void ScGraphicShell::ExecuteCompressGraphic( SAL_UNUSED_PARAMETER SfxRequest& )
{
    ScDrawView* pView = GetViewData().GetScDrawView();
    const SdrMarkList& rMarkList = pView->GetMarkedObjectList();

    if( rMarkList.GetMarkCount() == 1 )
    {
        SdrObject* pObj = rMarkList.GetMark( 0 )->GetMarkedSdrObj();

        if( auto pGraphicObj = dynamic_cast<SdrGrafObj*>( pObj) )
            if( pGraphicObj->GetGraphicType() == GraphicType::Bitmap )
            {
                CompressGraphicsDialog dialog(GetViewData().GetDialogParent(), pGraphicObj, GetViewData().GetBindings());
                if (dialog.run() == RET_OK)
                {
                    rtl::Reference<SdrGrafObj> pNewObject = dialog.GetCompressedSdrGrafObj();
                    SdrPageView* pPageView = pView->GetSdrPageView();
                    OUString aUndoString = pView->GetDescriptionOfMarkedObjects() + " Compress";
                    pView->BegUndo( aUndoString );
                    pView->ReplaceObjectAtView( pObj, *pPageView, pNewObject.get() );
                    pView->EndUndo();
                }
            }
    }

    Invalidate();
}

void ScGraphicShell::GetCropGraphicState( SfxItemSet& rSet )
{
    ScDrawView* pView = GetViewData().GetScDrawView();
    const SdrMarkList& rMarkList = pView->GetMarkedObjectList();
    bool bEnable = false;
    if( rMarkList.GetMarkCount() == 1 )
    {
        SdrObject* pObj = rMarkList.GetMark( 0 )->GetMarkedSdrObj();

        if( auto pGraphicObj = dynamic_cast<const SdrGrafObj*>( pObj) )
            if( pGraphicObj->GetGraphicType() == GraphicType::Bitmap )
                bEnable = true;
    }

    if( !bEnable )
        rSet.DisableItem( SID_OBJECT_CROP );
}

void ScGraphicShell::ExecuteCropGraphic( SAL_UNUSED_PARAMETER SfxRequest& )
{
    ScDrawView* pView = GetViewData().GetScDrawView();
    const SdrMarkList& rMarkList = pView->GetMarkedObjectList();

    if( rMarkList.GetMarkCount() == 1 )
    {
        SdrObject* pObj = rMarkList.GetMark( 0 )->GetMarkedSdrObj();

        if( auto pGraphicObj = dynamic_cast<const SdrGrafObj*>( pObj) )
            if( pGraphicObj->GetGraphicType() == GraphicType::Bitmap )
            {
                pView->SetEditMode(SdrViewEditMode::Edit);
                pView->SetDragMode(SdrDragMode::Crop);
            }
    }

    Invalidate();
}

void ScGraphicShell::ExecuteSaveGraphic( SfxRequest& rReq)
{
    ScDrawView* pView = GetViewData().GetScDrawView();
    const SdrMarkList& rMarkList = pView->GetMarkedObjectList();
    if( rMarkList.GetMarkCount() == 1 )
    {
        const SdrGrafObj* pObj = dynamic_cast<const SdrGrafObj*>(rMarkList.GetMark( 0 )->GetMarkedSdrObj());
        if( pObj && pObj->GetGraphicType() == GraphicType::Bitmap )
        {
            GraphicAttr aGraphicAttr = pObj->GetGraphicAttr();
            short nState = RET_CANCEL;
            vcl::Window* pWin = GetViewData().GetActiveWin();
            weld::Window* pWinFrame = pWin ? pWin->GetFrameWeld() : nullptr;
            if (aGraphicAttr != GraphicAttr()) // the image has been modified
            {
                if (pWin)
                {
                    nState = GraphicHelper::HasToSaveTransformedImage(pWinFrame);
                }
            }
            else
            {
                nState = RET_NO;
            }

            if (nState == RET_YES)
            {
                GraphicHelper::ExportGraphic(pWinFrame, pObj->GetTransformedGraphic(), "");
            }
            else if (nState == RET_NO)
            {
                const GraphicObject& aGraphicObject(pObj->GetGraphicObject());
                if (comphelper::LibreOfficeKit::isActive()) {
                    OUString sGrfNm;
                    const SfxStringItem* oName = rReq.GetArg<SfxStringItem>(SID_OX_SAVE_GRAPHIC);
                    if (oName) {
                        sGrfNm = oName->GetValue();
                        GraphicHelper::ExportGraphic(pWinFrame, aGraphicObject.GetGraphic(), sGrfNm);
                    }
                }
                else
                {
                    GraphicHelper::ExportGraphic(pWinFrame, aGraphicObject.GetGraphic(), "");
                }
            }
        }
    }

    Invalidate();
}

void ScGraphicShell::GetSaveGraphicState(SfxItemSet &rSet)
{
    ScDrawView* pView = GetViewData().GetScDrawView();
    const SdrMarkList& rMarkList = pView->GetMarkedObjectList();
    bool bEnable = false;
    if( rMarkList.GetMarkCount() == 1 )
    {
        SdrObject* pObj = rMarkList.GetMark( 0 )->GetMarkedSdrObj();

        if( auto pGraphicObj = dynamic_cast<const SdrGrafObj*>( pObj) )
            if( pGraphicObj->GetGraphicType() == GraphicType::Bitmap )
                bEnable = true;
    }

    if (GetObjectShell()->isExportLocked())
        bEnable = false;

    if( !bEnable )
        rSet.DisableItem( SID_SAVE_GRAPHIC );
}

void ScGraphicShell::ExecuteChangePicture( SfxRequest& rReq)
{
    ScDrawView* pView = GetViewData().GetScDrawView();
    const SdrMarkList& rMarkList = pView->GetMarkedObjectList();

    if( rMarkList.GetMarkCount() == 1 )
    {
        SdrObject* pObj = rMarkList.GetMark( 0 )->GetMarkedSdrObj();

        if( auto pGraphicObj = dynamic_cast<SdrGrafObj*>( pObj) )
            if( pGraphicObj->GetGraphicType() == GraphicType::Bitmap )
            {
                vcl::Window* pWin = GetViewData().GetActiveWin();
                SvxOpenGraphicDialog aDlg(ScResId(STR_INSERTGRAPHIC), pWin ? pWin->GetFrameWeld() : nullptr);

                bool run = false;
                if (comphelper::LibreOfficeKit::isActive())
                    run = true;
                else
                    run = aDlg.Execute() == ERRCODE_NONE? true: false;

                if( run )
                {
                    Graphic aGraphic;

                    ErrCode nError;
                    if (comphelper::LibreOfficeKit::isActive()) {
                        const SfxStringItem* aFullName = rReq.GetArg<SfxStringItem>(SID_OX_CHANGE_PICTURE);
                        if (aFullName) {
                            INetURLObject aURL;
                            aURL.SetSmartURL( aFullName->GetValue() );
                            OUString aFilterName = aURL.getExtension();
                            nError = GraphicFilter::LoadGraphic( aFullName->GetValue(), aFilterName, aGraphic, &GraphicFilter::GetGraphicFilter() );
                        }
                    }
                    else
                    {
                        nError = aDlg.GetGraphic(aGraphic);
                    }

                    /* Franklin.20231219: 1. nError may be uninitialized here!
                     * 2. "run" here is nonsense since if it's false it won't go into here */
                    if( nError == ERRCODE_NONE /* || run */)
                    {
                        rtl::Reference<SdrGrafObj> pNewObject(SdrObject::Clone(*pGraphicObj, pGraphicObj->getSdrModelFromSdrObject()));
                        pNewObject->SetGraphic( aGraphic );
                        SdrPageView* pPageView = pView->GetSdrPageView();
                        OUString aUndoString = pView->GetDescriptionOfMarkedObjects() + " Change";
                        pView->BegUndo( aUndoString );
                        pView->ReplaceObjectAtView( pObj, *pPageView, pNewObject.get() );
                        pView->EndUndo();
                    }
                }
            }
    }

    Invalidate();
}

void ScGraphicShell::GetChangePictureState(SfxItemSet &rSet)
{
    ScDrawView* pView = GetViewData().GetScDrawView();
    const SdrMarkList& rMarkList = pView->GetMarkedObjectList();
    bool bEnable = false;
    if( rMarkList.GetMarkCount() == 1 )
    {
        SdrObject* pObj = rMarkList.GetMark( 0 )->GetMarkedSdrObj();

        if( auto pGrafObj = dynamic_cast<const SdrGrafObj*>( pObj) )
            if( pGrafObj->GetGraphicType() == GraphicType::Bitmap )
                bEnable = true;
    }

    if( !bEnable )
        rSet.DisableItem( SID_CHANGE_PICTURE );
}


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
