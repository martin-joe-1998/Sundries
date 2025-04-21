# -*- coding: utf-8 -*-
# Maya 2018 only support python 2.7!!!!
import math
import random
import maya.cmds as cmds

# Rotate 3D vector "pos" "rot" degrees ont the axis "mode" (mode 0 = x, mode 1 = y, mode 2 = z)
# return float3 = [x, y, z] in general, return original 'pos' if mode err
def rot_position(pos, rot, mode):
    x, y, z = pos
    rot_rad = math.radians(rot)

    if mode == 0:  # axis X
        return [x,
                y * math.cos(rot_rad) - z * math.sin(rot_rad),
                y * math.sin(rot_rad) + z * math.cos(rot_rad)]
    elif mode == 1:  # axis Y
        return [z * math.sin(rot_rad) + x * math.cos(rot_rad),
                y,
                z * math.cos(rot_rad) - x * math.sin(rot_rad)]
    elif mode == 2:  # axis Z
        return [x * math.cos(rot_rad) - y * math.sin(rot_rad),
                x * math.sin(rot_rad) + y * math.cos(rot_rad),
                z]
    else:
        return pos


# Get object's index in its parent object(group).
# return 'i' as index, or '-1' if no result.
def ari_asset_changer_list_num(obj):
    # Get parent object
    parent = cmds.listRelatives(obj, parent=True, fullPath=True)
    
    # if there're no parent, then search from root.
    if not parent:
        children = cmds.ls(assemblies=True)
    # if find parent, then get all object in parent group 
    else:
        children = cmds.listRelatives(parent[0], children=True, fullPath=True)

    # trip to find
    for i, child in enumerate(children):
        if child == obj:
            return i
    return -1  # if no result


# copy all of the connection in target_obj(replaced) -> base_obj(new copy)
# include : destination connections & source connections
def ari_object_ani_copy(target_obj, base_obj):
    # Get destination connections from target_obj
    desti_conns = cmds.listConnections(target_obj, c=True, p=True, d=True, s=False) or []

    # copy every attribute from target -> base
    for i in range(0, len(desti_conns), 2):
        base_attr = desti_conns[i]  # eg. target_obj.translate
        target_attr = desti_conns[i + 1]  # eg. anotherNode.input
        suffix = base_attr[len(target_obj):]
        new_base_attr = base_obj + suffix
        target_node = target_attr.split('.')[0]
        connected = cmds.listConnections(new_base_attr, d=True, s=False)
        if not connected or target_node not in connected:
            cmds.connectAttr(new_base_attr, target_attr, force=True)

    # Get source connections from target_obj
    source_conns = cmds.listConnections(target_obj, c=True, p=True, d=False, s=True) or []

    for i in range(0, len(source_conns), 2):
        base_attr = source_conns[i]     # eg. anotherNode.output
        target_attr = source_conns[i+1] # eg. target_obj.translate
        suffix = target_attr[len(target_obj):]
        new_base_attr = base_obj + suffix
        target_node = base_attr.split('.')[0]
        connected = cmds.listConnections(new_base_attr, d=False, s=True)
        if not connected or target_node not in connected:
            cmds.connectAttr(base_attr, new_base_attr, force=True)


# Global Variable
AriAssetChangerTextField = "AriAssetChangerTextField"
AriAssetChangerList = "AriAssetChangerList"
# Global Variable


# write target's name into TextField and save it on optionVar, then reload child list
# When [Get] button pressed, this func will be triggered.
def ari_asset_changer_list_get():
    sel = cmds.ls(selection=True, transforms=True)
    if not sel:
        return
    group_name = sel[0]
    cmds.textField(AriAssetChangerTextField, edit=True, text=group_name)
    ari_asset_changer_reload()  # reload child list
    cmds.optionVar(sv=("AriAssetChanger_get", group_name))


# selete group which name was recorded in TextField, then clear TextField
def ari_asset_changer_list_group_sel():
    # Get TextField info
    text = cmds.textField(AriAssetChangerTextField, query=True, text=True)
    # return empty if no text
    if not text:
        return
    # selete & replace text object if exist
    if cmds.objExists(text):
        cmds.select(text, replace=True)
    cmds.textField(AriAssetChangerTextField, edit=True, text="")
    cmds.textScrollList(AriAssetChangerList, edit=True, removeAll=True)


# pickWalk left/right to next object, then reload TextField and list
# mode : 0 = right(next), 1 = left(prev)
def ari_asset_changer_list_next_sel(mode):
    sel_obj = cmds.ls(selection=True, transforms=True)
    
    # clear UI status(current TextField & list)
    ari_asset_changer_list_group_sel()
    
    # pickWalk to adjacent object
    if mode == 0:
        cmds.pickWalk(direction="right")
    elif mode == 1:
        cmds.pickWalk(direction="left")
    
    # reload TextField and list with new object
    ari_asset_changer_list_get()

    # recover original selection status
    if sel_obj:
        cmds.select(sel_obj, replace=True)


# return object name without group path(|)
def ari_asset_changer_base_name(obj):
    return obj.split('|')[-1] if obj else ""


# search object name in textScrollList by currently seleted object, and selete it
def ari_asset_changer_list_jump():
    # get seleted obj
    sel_obj = cmds.ls(selection=True, transforms=True)
    if not sel_obj:
        return

    # get obj name
    obj_name = ari_asset_changer_base_name(sel_obj[-1])

    # get all name in textScrollList
    all_list = cmds.textScrollList(AriAssetChangerList, query=True, allItems=True) or []
    all_base_list = [ari_asset_changer_base_name(name) for name in all_list]

    # search & match obj name from last letter
    matched_id = -1
    for i in reversed(range(1, len(obj_name) + 1)):
        prefix = obj_name[-i:]
        for j, list_name in enumerate(all_base_list):
            if list_name[-i:] == prefix:
                matched_id = j
                break
        if matched_id != -1:
            break

    if matched_id != -1:
        cmds.textScrollList(AriAssetChangerList, edit=True, deselectAll=True)
        cmds.textScrollList(AriAssetChangerList, edit=True, selectIndexedItem=(matched_id + 1,))


# replace 'target' with 'change object' and maintain all attribute
"""
Args:
        change_obj (str): Substitute Object
        target (str): Object to be replace
        instance (bool): Instancing(1) or copy(0)
        rename (bool): rename(1) or original name(0)
        childsave (bool): keep all child(1) or not(0)
        connect (bool): copy target's connection to change_obj or not
Returns:
        str: Substitute object's name
"""
def ari_object_change(change_obj, target, instance, rename, childsave, connect):
    pos = cmds.xform(target, q=True, ws=True, rp=True)
    rot = cmds.getAttr("{}.rotate".format(target))[0]
    scale = cmds.getAttr("{}.scale".format(target))[0]
    parent = cmds.listRelatives(target, parent=True, fullPath=True)
    target_num = ari_asset_changer_list_num(target)

    if instance:
        dup_obj = cmds.duplicate(change_obj, rr=True, instanceLeaf=True)
    else:
        dup_obj = cmds.duplicate(change_obj, rr=True)

    children = []
    if childsave:
        children = cmds.listRelatives(target, children=True, type="transform", fullPath=True) or []
        if children:
            children = cmds.parent(children, world=True)

    if parent:
        current_parent = cmds.listRelatives(dup_obj[0], parent=True, fullPath=True)
        if not current_parent or current_parent[0] != parent[0]:
            dup_obj = cmds.parent(dup_obj[0], parent[0])
        else:
            dup_obj = [dup_obj[0]]
    else:
        dup_obj = cmds.parent(dup_obj[0], world=True)

    cmds.reorder(dup_obj[0], relative=target_num + 1)
    cmds.move(pos[0], pos[1], pos[2], dup_obj[0], rpr=True)
    cmds.setAttr("{}.rotate".format(dup_obj[0]), *rot)
    cmds.setAttr("{}.scale".format(dup_obj[0]), *scale)

    if childsave and children:
        cmds.parent(children, dup_obj[0])

    if connect:
        ari_object_ani_copy(target, dup_obj[0])

    cmds.delete(target)

    if rename:
        name_parts = target.split("|")
        dup_obj[0] = cmds.rename(dup_obj[0], name_parts[-1])

    return dup_obj[0]


# quick access to main logic func when user click [selete] in list
def ari_asset_changer_select_go():
    ari_asset_changer_go(0)


# main logic func
# execute 'changer' base on seleted obj, asset, and other options
"""
Args:
    mode (int): 0 = selectGo, 1 = Random
"""
def ari_asset_changer_go(mode):
    # Read-in UI elements
    instance  = cmds.checkBox(AriAssetChangerCheckBox[0], query=True, value=True)
    rename    = cmds.checkBox(AriAssetChangerCheckBox[1], query=True, value=True)
    childsave = cmds.checkBox(AriAssetChangerCheckBox[2], query=True, value=True)
    connect   = cmds.checkBox(AriAssetChangerCheckBox[3], query=True, value=True)
    parent_name = cmds.textField(AriAssetChangerTextField, query=True, text=True)
    
    sel_objs = cmds.ls(selection=True, transforms=True) or []
    full_sel = cmds.ls(selection=True) or []
    vers = cmds.filterExpand(full_sel, selectionMask=31) or []

    change_obj = cmds.textScrollList(AriAssetChangerList, query=True, selectItem=True) or []
    all_items = cmds.textScrollList(AriAssetChangerList, query=True, allItems=True) or []

    if len(change_obj) >= 2 and mode == 0:
        return
    if len(change_obj) == 0 and mode == 0:
        return

    # === Replace the selected transform object ===
    changed_objs = []
    if sel_objs:
        for target in sel_objs:
            parents = cmds.listRelatives(target, parent=True, fullPath=True)
            if not parents or parents[0] != parent_name:
                if len(change_obj) <= 1 and mode == 1:
                    cmds.textScrollList(AriAssetChangerList, edit=True, deselectAll=True)
                    change_obj = cmds.textScrollList(AriAssetChangerList, query=True, allItems=True)
                select_id = random.randint(0, len(change_obj) - 1)
                new_obj = ari_object_change(
                    change_obj[select_id], target, instance, rename, childsave, connect
                )
                changed_objs.append(new_obj)
            else:
                print("自身の階層??のためスキ??プ\n")

    # === Vertex Replacement Mode ===
    created_objs = []
    if vers:
        direction_enabled = cmds.checkBox(AriAssetChangerRadio[4], query=True, value=True)
        direction_xyz = [0, 1, 0]
        if cmds.radioButton(AriAssetChangerRadio[0], query=True, select=True): direction_xyz = [1, 0, 0]
        if cmds.radioButton(AriAssetChangerRadio[1], query=True, select=True): direction_xyz = [0, 1, 0]
        if cmds.radioButton(AriAssetChangerRadio[2], query=True, select=True): direction_xyz = [0, 0, 1]
        if cmds.checkBox(AriAssetChangerRadio[3], query=True, value=True): direction_xyz = list(map(lambda x: -x, direction_xyz))

        center_mode = False
        if not center_mode:
            for ver in vers:
                obj = cmds.listRelatives(cmds.polyListComponentConversion(ver, toTransform=True)[0], parent=True, fullPath=True)[0]
                pos = cmds.pointPosition(ver, world=True)

                # Vertex Normal Average
                nor = cmds.polyNormalPerVertex(ver, query=True, xyz=True)
                nor_vector = [sum(nor[i::3])/len(nor[::3]) for i in range(3)]

                # level rotation
                this_obj = obj
                while this_obj:
                    rot = cmds.getAttr("{}.rotate".format(this_obj))[0]
                    nor_vector = rot_position(nor_vector, rot[0], 0)
                    nor_vector = rot_position(nor_vector, rot[1], 1)
                    nor_vector = rot_position(nor_vector, rot[2], 2)
                    parent = cmds.listRelatives(this_obj, parent=True, fullPath=True)
                    this_obj = parent[0] if parent else ""

                # Direction Alignt Rotation
                if direction_enabled:
                    eul_rot = cmds.angleBetween(euler=True,
                                                v1=direction_xyz,
                                                v2=nor_vector)
                else:
                    eul_rot = [0, 0, 0]

                if len(change_obj) <= 1 and mode == 1:
                    cmds.textScrollList(AriAssetChangerList, edit=True, deselectAll=True)
                    change_obj = cmds.textScrollList(AriAssetChangerList, query=True, allItems=True)
                select_id = random.randint(0, len(change_obj) - 1)

                if instance:
                    dup = cmds.duplicate(change_obj[select_id], rr=True, instanceLeaf=True)
                else:
                    dup = cmds.duplicate(change_obj[select_id], rr=True)
                cmds.move(pos[0], pos[1], pos[2], dup[0], rpr=True)
                cmds.setAttr("{}.rotate".format(dup[0]), eul_rot[0], eul_rot[1], eul_rot[2])
                cmds.parent(dup[0], world=True)
                created_objs.append(dup[0])
        else:
            pass

        cmds.group(created_objs)

    # reload seleted obj
    cmds.select(changed_objs + created_objs)


# Get obj name: "|group1|group2|cube" -> "cube"
def ari_asset_changer_base_name(obj):
    return obj.split('|')[-1] if obj else ""


# reload all of the child obj under TextField sel to Scroll list
# reload available asset
def ari_asset_changer_reload():
    # clear list
    cmds.textScrollList(AriAssetChangerList, edit=True, removeAll=True)

    # get group name
    group_name = cmds.textField(AriAssetChangerTextField, query=True, text=True)
    if not group_name or not cmds.objExists(group_name):
        return

    # obtain all child obj
    children = cmds.listRelatives(group_name, children=True, fullPath=True) or []

    # add all child to Scroll list
    for child in children:
        cmds.textScrollList(AriAssetChangerList, edit=True, append=[child])


# control on/off of direction alignment
def ari_asset_changer_ui_change():
    enabled = cmds.checkBox(AriAssetChangerRadio[4], query=True, value=True)

    # On/Off X/Y/Z/- controls
    cmds.radioButton(AriAssetChangerRadio[0], edit=True, enable=enabled)
    cmds.radioButton(AriAssetChangerRadio[1], edit=True, enable=enabled)
    cmds.radioButton(AriAssetChangerRadio[2], edit=True, enable=enabled)
    cmds.checkBox(AriAssetChangerRadio[3], edit=True, enable=enabled)

    # Calculate seleted direction
    select_xyz = 1
    if cmds.radioButton(AriAssetChangerRadio[0], query=True, select=True): select_xyz = 0
    if cmds.radioButton(AriAssetChangerRadio[1], query=True, select=True): select_xyz = 1
    if cmds.radioButton(AriAssetChangerRadio[2], query=True, select=True): select_xyz = 2
    if cmds.checkBox(AriAssetChangerRadio[3], query=True, value=True): select_xyz += 3

    # save as optionVar
    cmds.optionVar(iv=("AriAssetChanger_direction", int(enabled)))
    cmds.optionVar(iv=("AriAssetChanger_direXYZ", select_xyz))

    print("Direction Align Mode: {}".format(select_xyz))


# Build the whole PlugIn UI window
def ari_asset_changer():
    # load saved option parameters
    data_get = cmds.optionVar(q="AriAssetChanger_get") if cmds.optionVar(exists="AriAssetChanger_get") else ""
    data_frame_close = cmds.optionVar(q="AriAssetChanger_frameClose") if cmds.optionVar(exists="AriAssetChanger_frameClose") else 1
    data_instance = cmds.optionVar(q="AriAssetChanger_instance") if cmds.optionVar(exists="AriAssetChanger_instance") else 0
    data_rename = cmds.optionVar(q="AriAssetChanger_rename") if cmds.optionVar(exists="AriAssetChanger_rename") else 0
    data_child = cmds.optionVar(q="AriAssetChanger_child") if cmds.optionVar(exists="AriAssetChanger_child") else 0
    data_node = cmds.optionVar(q="AriAssetChanger_node") if cmds.optionVar(exists="AriAssetChanger_node") else 1
    data_direction = cmds.optionVar(q="AriAssetChanger_direction") if cmds.optionVar(exists="AriAssetChanger_direction") else 1
    data_direXYZ = cmds.optionVar(q="AriAssetChanger_direXYZ") if cmds.optionVar(exists="AriAssetChanger_direXYZ") else 1

    if cmds.window("AriAssetChanger", exists=True):
        cmds.deleteUI("AriAssetChanger")

    cmds.window("AriAssetChanger", title="AriAssetChanger", widthHeight=(180, 100))
    form = cmds.formLayout()

    # === Top [Get] col ===
    get_col = cmds.columnLayout(adjustableColumn=True)

    # Sceond row(TextField + < > button)
    row2 = cmds.formLayout()
    global AriAssetChangerTextField
    AriAssetChangerTextField = cmds.textField(height=24, text=data_get, editable=False)
    left_btn = cmds.button(label="<", width=20, command=lambda *_: ari_asset_changer_list_next_sel(1))
    right_btn = cmds.button(label=">", width=20, command=lambda *_: ari_asset_changer_list_next_sel(0))

    cmds.formLayout(row2, edit=True,
        attachForm=[
            (right_btn, 'right', 0),
            (AriAssetChangerTextField, 'left', 0),
        ],
        attachControl=[
            (left_btn, 'right', 0, right_btn),
            (AriAssetChangerTextField, 'right', 0, left_btn),
        ]
    )
    cmds.setParent('..')  # end formLayout

    # First row(Get / Reload button)
    # First row(Get / Reload button)
    cmds.rowLayout(numberOfColumns=2, adjustableColumn=2, columnWidth2=(80, 80), columnAlign2=("center", "center"))
    cmds.button(label="Get", width=80, bgc=(0.4, 0.8, 0.4), command=lambda *_: ari_asset_changer_list_get())
    cmds.button(label="Reload", width=80, command=lambda *_: ari_asset_changer_reload())
    cmds.setParent('..')


    cmds.setParent('..')  # end column

    # === Scroll List ===
    global AriAssetChangerList
    AriAssetChangerList = cmds.textScrollList(allowMultiSelection=True, selectCommand=lambda *_: ari_asset_changer_select_go())

    # === Down side button ===
    etc_col = cmds.columnLayout(adjustableColumn=True)
    cmds.button(label="Jump", width=80, command=lambda *_: ari_asset_changer_list_jump())
    cmds.button(label="Random", width=80, command=lambda *_: ari_asset_changer_go(1))

    # === Option window (foldable)===
    frame = cmds.frameLayout(label="Option", collapsable=True, collapse=bool(data_frame_close),
                             collapseCommand=lambda: cmds.optionVar(iv=("AriAssetChanger_frameClose", 1)),
                             expandCommand=lambda: cmds.optionVar(iv=("AriAssetChanger_frameClose", 0)))
    cmds.columnLayout(adjustableColumn=True)

    global AriAssetChangerCheckBox, AriAssetChangerRadio
    AriAssetChangerCheckBox = [
        cmds.checkBox(label=u"インスタンス", value=bool(data_instance),
                      onc=lambda *_: cmds.optionVar(iv=("AriAssetChanger_instance", 1)),
                      ofc=lambda *_: cmds.optionVar(iv=("AriAssetChanger_instance", 0))),
        cmds.checkBox(label=u"リネーム", value=bool(data_rename), annotation=u"コピー先のオブジェクト名を使用",
                      onc=lambda *_: cmds.optionVar(iv=("AriAssetChanger_rename", 1)),
                      ofc=lambda *_: cmds.optionVar(iv=("AriAssetChanger_rename", 0))),
        cmds.checkBox(label=u"子を保持", value=bool(data_child),
                      onc=lambda *_: cmds.optionVar(iv=("AriAssetChanger_child", 1)),
                      ofc=lambda *_: cmds.optionVar(iv=("AriAssetChanger_child", 0))),
        cmds.checkBox(label=u"ノード接続を保持", value=bool(data_node), annotation=u"コピー先に接続されているノードをつなぐ",
                      onc=lambda *_: cmds.optionVar(iv=("AriAssetChanger_node", 1)),
                      ofc=lambda *_: cmds.optionVar(iv=("AriAssetChanger_node", 0)))
    ]

    cmds.rowLayout(numberOfColumns=2)
    cmds.text(label="頂点配置向き")
    AriAssetChangerRadio = [None]*8  # pre-asign
    AriAssetChangerRadio[4] = cmds.checkBox(label="", value=bool(data_direction), cc=lambda *_: ari_asset_changer_ui_change())
    cmds.setParent('..')

    cmds.radioCollection()
    cmds.rowLayout(numberOfColumns=6)
    AriAssetChangerRadio[0] = cmds.radioButton(label="X", width=32, bgc=(0.4, 0, 0), onc=lambda *_: ari_asset_changer_ui_change())
    AriAssetChangerRadio[1] = cmds.radioButton(label="Y", width=32, bgc=(0, 0.4, 0), onc=lambda *_: ari_asset_changer_ui_change())
    AriAssetChangerRadio[2] = cmds.radioButton(label="Z", width=32, bgc=(0, 0, 0.4), onc=lambda *_: ari_asset_changer_ui_change())
    AriAssetChangerRadio[3] = cmds.checkBox(label="-", cc=lambda *_: ari_asset_changer_ui_change())

    cmds.setParent('..')
    cmds.setParent('..')
    cmds.setParent('..')
    cmds.setParent('..')

    # === Form layout ===
    cmds.formLayout(form, edit=True,
        attachForm=[
            (get_col, 'top', 0), (get_col, 'left', 0), (get_col, 'right', 0),
            (etc_col, 'bottom', 0), (etc_col, 'left', 0), (etc_col, 'right', 0),
            (AriAssetChangerList, 'left', 0), (AriAssetChangerList, 'right', 0),
        ],
        attachControl=[
            (AriAssetChangerList, 'top', 0, get_col),
            (AriAssetChangerList, 'bottom', 0, etc_col)
        ]
    )

    cmds.showWindow("AriAssetChanger")

    # === Recover direciton opt ===
    sel_xyz = data_direXYZ % 3
    sel_minus = data_direXYZ // 3
    cmds.radioButton(AriAssetChangerRadio[sel_xyz], edit=True, select=True)
    cmds.checkBox(AriAssetChangerRadio[3], edit=True, value=bool(sel_minus))

    # Activate on/off, reload list
    ari_asset_changer_ui_change()
    ari_asset_changer_reload()