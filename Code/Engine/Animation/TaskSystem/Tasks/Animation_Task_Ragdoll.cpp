#include "Animation_Task_Ragdoll.h"
#include "Engine/Animation/AnimationBlender.h"
#include "Engine/Physics/PhysicsRagdoll.h"
#include "Base/Profiling.h"

//-------------------------------------------------------------------------

namespace EE::Animation::Tasks
{
    RagdollSetPoseTask::RagdollSetPoseTask( Physics::Ragdoll* pRagdoll, TaskSourceID sourceID, TaskIndex sourceTaskIdx, InitOption initOption )
        : Task( sourceID, TaskUpdateStage::PrePhysics, { sourceTaskIdx } )
        , m_pRagdoll( pRagdoll )
        , m_initOption( initOption )
    {
        EE_ASSERT( pRagdoll != nullptr );
        EE_ASSERT( sourceTaskIdx != InvalidIndex );
    }

    void RagdollSetPoseTask::Execute( TaskContext const& context )
    {
        //EE_PROFILE_FUNCTION_ANIMATION();

        auto pSourceBuffer = TransferDependencyPoseBuffer( context, 0 );
        pSourceBuffer->GetPrimaryPose()->CalculateGlobalTransforms();
        m_pRagdoll->Update( context.m_deltaTime, context.m_worldTransform, pSourceBuffer->GetPrimaryPose(), m_initOption == InitializeBodies );
        MarkTaskComplete( context );
    }

    //-------------------------------------------------------------------------

    RagdollGetPoseTask::RagdollGetPoseTask( Physics::Ragdoll* pRagdoll, TaskSourceID sourceID, TaskIndex sourceTaskIdx, float const physicsBlendWeight )
        : Task( sourceID, TaskUpdateStage::PostPhysics, { sourceTaskIdx } )
        , m_pRagdoll( pRagdoll )
        , m_physicsBlendWeight( physicsBlendWeight )
    {
        EE_ASSERT( pRagdoll != nullptr );
        EE_ASSERT( sourceTaskIdx != InvalidIndex );
    }

    RagdollGetPoseTask::RagdollGetPoseTask( Physics::Ragdoll* pRagdoll, TaskSourceID sourceID )
        : Task( sourceID, TaskUpdateStage::PostPhysics, {} )
        , m_pRagdoll( pRagdoll )
    {
        EE_ASSERT( pRagdoll != nullptr );
    }

    void RagdollGetPoseTask::Execute( TaskContext const& context )
    {
        //EE_PROFILE_FUNCTION_ANIMATION();

        if ( context.m_dependencies.empty() )
        {
            auto pResultBuffer = GetNewPoseBuffer( context );
            auto pPrimaryPose = pResultBuffer->GetPrimaryPose();
            pPrimaryPose->CalculateGlobalTransforms();
            m_pRagdoll->GetPose( context.m_worldTransform, pPrimaryPose );
        }
        else // Potentially blend the poses
        {
            // Get the animation pose
            auto pResultBuffer = TransferDependencyPoseBuffer( context, 0 );
            auto pPrimaryPose = pResultBuffer->GetPrimaryPose();

            // Overwrite it with the physics pose
            if ( Math::IsNearEqual( m_physicsBlendWeight, 1.0f, Math::LargeEpsilon ) )
            {
                pPrimaryPose->CalculateGlobalTransforms();
                m_pRagdoll->GetPose( context.m_worldTransform, pPrimaryPose );
            }
            else if ( m_physicsBlendWeight > Math::LargeEpsilon )
            {
                // Get a temporary pose we can use
                PoseBuffer* pTempBuffer = nullptr;
                int8_t const tmpBufferIdx = GetTemporaryPoseBuffer( context, pTempBuffer );
                EE_ASSERT( pTempBuffer != nullptr );
                Pose* pTempPrimaryPose = pTempBuffer->GetPrimaryPose();
                pTempPrimaryPose->CalculateGlobalTransforms();

                // Get the ragdoll pose and blend it with the animation pose
                m_pRagdoll->GetPose( context.m_worldTransform, pTempPrimaryPose );
                Animation::Blender::LocalBlend( context.m_skeletonLOD, pPrimaryPose, pTempPrimaryPose, m_physicsBlendWeight, nullptr, pPrimaryPose );

                ReleaseTemporaryPoseBuffer( context, tmpBufferIdx );
            }
        }

        MarkTaskComplete( context );
    }

    #if EE_DEVELOPMENT_TOOLS
    String RagdollGetPoseTask::GetDebugText() const
    {
        if ( m_physicsBlendWeight < 1.0f )
        {
            return String( String::CtorSprintf(), "Get Ragdoll Pose (%.2f)", m_physicsBlendWeight );
        }
        else
        {
            return "Get Ragdoll Pose";
        }
    }
    #endif
}