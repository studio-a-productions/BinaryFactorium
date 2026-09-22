/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/

#include <BinF/Engine.hpp>

namespace BinF::Game {
    /*
        IDs used for cold <-> hot switching and keeping deterministic content
    */

    using NodeID = u32;
    using EdgeID = u32;

    struct Node;
    struct Edge;

    struct NodeRef {
        NodeID m_Id;
        Node* m_Ptr;
    };
    struct EdgeRef {
        EdgeID m_Id;
        Edge* m_Ptr;
    };


    constexpr NodeID InvalidNodeID = u32m;
    constexpr EdgeID InvalidEdgeID = u32m;

    constexpr NodeRef InvalidNodeRef = {
        InvalidNodeID,
        nullptr
    };
    constexpr EdgeRef InvalidEdgeRef = {
        InvalidEdgeID,
        nullptr
    };

    constexpr u8 MaxNodeIn  = 3U;
    constexpr u8 MaxNodeOut = 3U;

    enum class ItemType : BinF::u16 {
        Silicon = 0,
        SiliconCrystal,
        Copper,
        CopperSheet,
        CopperWire,
        Gold,
        PureGold,
        Limestone,
        Concrete,
        Iron,
        PureIron,
        IronBar,
        IronPlate,
        Steel,
        Coal,
        Biomatter,
        Biofuel,
        Uranium,
        UraniumPlus,
        FCHE,
        Plastic,
        Cobalt,
        Magnet,
        Lithium,
        Acid,

        ComputeChipMk1=1024,
        Cable,
        SteelColumn,
        SteelFrame,
        Container,
        NuclearRod,
        BatteryCell,
        BatteryPack
    };


    enum class NodeType : BinF::u8 {
        Actor,      // does stuff on its own that interacts
        Reactor,    // does stuff after input/output (like a filter)
        Passive     // constant behaviour
    };

    struct Edge {
        bool Open;
        u16 /* change to u32 if (bugs) */
        Length;     // = capacity (what happens right before int overflow? -> new node with zero delay, easy! and when node overflows, edge "could" store capacity too...)
        
        u32 
        Delay;  // ms from Begin to End (a kind of throughput)
        EdgeID
        ID;   // specific edgeid


        NodeRef Begin;
        NodeRef End;
    };

    struct Node {
        bool 
        Open;
        
        NodeType 
        Type;

        u32 
        Delay; // ms to out
        NodeID
        ID; // specific nodeid

        // can be edgeids, but would cost lookup time
        EdgeRef In[MaxNodeIn];
        EdgeRef Out[MaxNodeOut];
    };
}