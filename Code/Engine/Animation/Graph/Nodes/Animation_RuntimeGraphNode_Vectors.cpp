#include "Animation_RuntimeGraphNode_Vectors.h"
#include "System/Log.h"
#include "System/Math/MathHelpers.h"

//-------------------------------------------------------------------------

namespace EE::Animation::GraphNodes
{
    void VectorInfoNode::Settings::InstantiateNode( InstantiationContext const& context, InstantiationOptions options ) const
    {
        auto pNode = CreateNode<VectorInfoNode>( context, options );
        context.SetNodePtrFromIndex( m_inputValueNodeIdx, pNode->m_pInputValueNode );
    }

    void VectorInfoNode::InitializeInternal( GraphContext& context )
    {
        EE_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        FloatValueNode::InitializeInternal( context );
        m_pInputValueNode->Initialize( context );
    }

    void VectorInfoNode::ShutdownInternal( GraphContext& context )
    {
        EE_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        m_pInputValueNode->Shutdown( context );
        FloatValueNode::ShutdownInternal( context );
    }

    void VectorInfoNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        EE_ASSERT( context.IsValid() && m_pInputValueNode != nullptr );
        auto pSettings = GetSettings<VectorInfoNode>();

        if ( !WasUpdated( context ) )
        {
            MarkNodeActive( context );

            Vector const inputVector = m_pInputValueNode->GetValue<Vector>( context );
            switch ( pSettings->m_desiredInfo )
            {
            case Info::X: m_value = inputVector.m_x; break;
            case Info::Y: m_value = inputVector.m_y; break;
            case Info::Z: m_value = inputVector.m_z; break;
            case Info::W: m_value = inputVector.m_w; break;

            case Info::Length:
            {
                m_value = inputVector.GetLength3();
            }
            break;

            case Info::AngleHorizontal:
            {
                m_value = (float) Math::GetYawAngleBetweenVectors( Vector::WorldForward, inputVector );
            }
            break;

            case Info::AngleVertical:
            {
                m_value = (float) Quaternion::FromRotationBetweenVectors( Vector::WorldForward, inputVector ).ToEulerAngles().GetPitch();
            }
            break;

            case Info::SizeHorizontal:
            {
                m_value = inputVector.GetLength3();
            }
            break;

            case Info::SizeVertical:
            {
                m_value = Vector( inputVector.m_x, 0.0f, inputVector.m_z ).GetLength3();
            }
            break;
            }
        }

        *reinterpret_cast<float*>( pOutValue ) = m_value;
    }

    //-------------------------------------------------------------------------

    void VectorCreateNode::Settings::InstantiateNode( InstantiationContext const& context, InstantiationOptions options ) const
    {
        auto pNode = CreateNode<VectorCreateNode>( context, options );
        context.SetOptionalNodePtrFromIndex( m_inputVectorValueNodeIdx, pNode->m_pInputXValueNode );
        context.SetOptionalNodePtrFromIndex( m_inputValueXNodeIdx, pNode->m_pInputXValueNode );
        context.SetOptionalNodePtrFromIndex( m_inputValueYNodeIdx, pNode->m_pInputYValueNode );
        context.SetOptionalNodePtrFromIndex( m_inputValueZNodeIdx, pNode->m_pInputZValueNode );
    }

    void VectorCreateNode::InitializeInternal( GraphContext& context )
    {
        VectorValueNode::InitializeInternal( context );

        if ( m_pInputVectorValueNode != nullptr )
        {
            m_pInputVectorValueNode->Initialize( context );
        }

        if ( m_pInputXValueNode != nullptr )
        {
            m_pInputXValueNode->Initialize( context );
        }

        if ( m_pInputYValueNode != nullptr )
        {
            m_pInputYValueNode->Initialize( context );
        }

        if ( m_pInputZValueNode != nullptr )
        {
            m_pInputZValueNode->Initialize( context );
        }
    }

    void VectorCreateNode::ShutdownInternal( GraphContext& context )
    {
        if ( m_pInputVectorValueNode != nullptr )
        {
            m_pInputVectorValueNode->Shutdown( context );
        }

        if ( m_pInputXValueNode != nullptr )
        {
            m_pInputXValueNode->Shutdown( context );
        }

        if ( m_pInputYValueNode != nullptr )
        {
            m_pInputYValueNode->Shutdown( context );
        }

        if ( m_pInputZValueNode != nullptr )
        {
            m_pInputZValueNode->Shutdown( context );
        }

        VectorValueNode::ShutdownInternal( context );
    }

    void VectorCreateNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        if ( !WasUpdated( context ) )
        {
            MarkNodeActive( context );

            //-------------------------------------------------------------------------

            if ( m_pInputVectorValueNode != nullptr )
            {
                m_value = m_pInputVectorValueNode->GetValue<Vector>( context );
            }
            else
            {
                m_value = Vector::Zero;
            }

            //-------------------------------------------------------------------------

            if ( m_pInputXValueNode != nullptr )
            {
                m_value.m_x = m_pInputXValueNode->GetValue<float>( context );
            }

            if ( m_pInputYValueNode != nullptr )
            {
                m_value.m_y = m_pInputYValueNode->GetValue<float>( context );
            }

            if ( m_pInputZValueNode != nullptr )
            {
                m_value.m_z = m_pInputZValueNode->GetValue<float>( context );
            }
        }

        *reinterpret_cast<Vector*>( pOutValue ) = m_value;
    }

    //-------------------------------------------------------------------------

    void VectorNegateNode::Settings::InstantiateNode( InstantiationContext const& context, InstantiationOptions options ) const
    {
        auto pNode = CreateNode<VectorNegateNode>( context, options );
        context.SetNodePtrFromIndex( m_inputValueNodeIdx, pNode->m_pInputValueNode );
    }

    void VectorNegateNode::InitializeInternal( GraphContext& context )
    {
        EE_ASSERT( m_pInputValueNode != nullptr );
        VectorValueNode::InitializeInternal( context );
        m_pInputValueNode->Initialize( context );
    }

    void VectorNegateNode::ShutdownInternal( GraphContext& context )
    {
        EE_ASSERT( m_pInputValueNode != nullptr );
        m_pInputValueNode->Shutdown( context );
        VectorValueNode::ShutdownInternal( context );
    }

    void VectorNegateNode::GetValueInternal( GraphContext& context, void* pOutValue )
    {
        EE_ASSERT( m_pInputValueNode != nullptr );

        if ( !WasUpdated( context ) )
        {
            MarkNodeActive( context );
            m_value = m_pInputValueNode->GetValue<Vector>( context ).GetNegated();
        }

        *reinterpret_cast<Vector*>( pOutValue ) = m_value;
    }
}