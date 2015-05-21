#pragma once
#include "explore_interop.h"

/*!
*  \addtogroup MultiWorldTestingCsharp
*  @{
*/
namespace MultiWorldTesting {

	/// <summary>
	/// The epsilon greedy exploration class.
	/// </summary>
	/// <remarks>
	/// This is a good choice if you have no idea which actions should be preferred.
	/// Epsilon greedy is also computationally cheap.
	/// </remarks>
	/// <typeparam name="Ctx">The Context type.</typeparam>
	generic <class Ctx>
	public ref class EpsilonGreedyExplorerMultiAction : public IExplorerMultiAction<Ctx>, public IConsumePolicyMultiAction<Ctx>, public PolicyCallback<Ctx>
	{
	public:
		/// <summary>
		/// The constructor is the only public member, because this should be used with the MwtExplorer.
		/// </summary>
		/// <param name="defaultPolicy">A default function which outputs an action given a context.</param>
		/// <param name="epsilon">The probability of a random exploration.</param>
		/// <param name="numActions">The number of actions to randomize over.</param>
		EpsilonGreedyExplorerMultiAction(IPolicyMultiAction<Ctx>^ defaultPolicy, float epsilon, UInt32 numActions)
		{
			this->defaultPolicy = defaultPolicy;
			m_explorer = new NativeMultiWorldTesting::EpsilonGreedyExplorerMultiAction<NativeContext>(*GetNativePolicy(), epsilon, (u32)numActions);
		}

        /// <summary>
        /// Initializes an epsilon greedy explorer with variable number of actions.
        /// </summary>
        /// <param name="defaultPolicy">A default function which outputs an action given a context.</param>
        /// <param name="epsilon">The probability of a random exploration.</param>
        EpsilonGreedyExplorerMultiAction(IPolicyMultiAction<Ctx>^ defaultPolicy, float epsilon)
        {
            this->defaultPolicy = defaultPolicy;
            m_explorer = new NativeMultiWorldTesting::EpsilonGreedyExplorerMultiAction<NativeContext>(*GetNativePolicy(), epsilon);
        }

		~EpsilonGreedyExplorerMultiAction()
		{
			delete m_explorer;
		}

        virtual void UpdatePolicy(IPolicyMultiAction<Ctx>^ newPolicy)
        {
            this->defaultPolicy = newPolicy;
        }

        virtual void EnableExplore(bool explore)
        {
            m_explorer->Enable_Explore(explore);
        }

	internal:
		virtual void InvokePolicyCallback(Ctx context, cli::array<UInt32>^ actions, int index) override
		{
            cli::array<UInt32>^ defaultActions = defaultPolicy->ChooseAction(context);
            
            if (defaultActions == nullptr)
            {
                throw gcnew NullReferenceException("List of actions returned by default policy is null.");
            }
            if (defaultActions->Length < actions->Length)
            {
                throw gcnew InvalidDataException("Number of actions returned by default policy is unexpected. Expected: " + actions->Length + ", Actual: " + defaultActions->Length);
            }

            // TODO: possible to remove the copy by requiring users to fill in a preallocated array instead.
            Array::Copy(defaultActions, actions, actions->Length);
		}

		NativeMultiWorldTesting::EpsilonGreedyExplorerMultiAction<NativeContext>* Get()
		{
			return m_explorer;
		}

	private:
		IPolicyMultiAction<Ctx>^ defaultPolicy;
		NativeMultiWorldTesting::EpsilonGreedyExplorerMultiAction<NativeContext>* m_explorer;
	};

	/// <summary>
	/// The tau-first exploration class.
	/// </summary>
	/// <remarks>
	/// The tau-first explorer collects precisely tau uniform random
	/// exploration events, and then uses the default policy. 
	/// </remarks>
	/// <typeparam name="Ctx">The Context type.</typeparam>
	generic <class Ctx>
    public ref class TauFirstExplorerMultiAction : public IExplorerMultiAction<Ctx>, public IConsumePolicyMultiAction<Ctx>, public PolicyCallback<Ctx>
	{
	public:
		/// <summary>
		/// The constructor is the only public member, because this should be used with the MwtExplorer.
		/// </summary>
		/// <param name="defaultPolicy">A default policy after randomization finishes.</param>
		/// <param name="tau">The number of events to be uniform over.</param>
		/// <param name="numActions">The number of actions to randomize over.</param>
		TauFirstExplorerMultiAction(IPolicyMultiAction<Ctx>^ defaultPolicy, UInt32 tau, UInt32 numActions)
		{
			this->defaultPolicy = defaultPolicy;
			m_explorer = new NativeMultiWorldTesting::TauFirstExplorerMultiAction<NativeContext>(*GetNativePolicy(), tau, (u32)numActions);
		}

        /// <summary>
        /// Initializes a tau-first explorer with variable number of actions.
        /// </summary>
        /// <param name="defaultPolicy">A default policy after randomization finishes.</param>
        /// <param name="tau">The number of events to be uniform over.</param>
        TauFirstExplorerMultiAction(IPolicyMultiAction<Ctx>^ defaultPolicy, UInt32 tau)
        {
            this->defaultPolicy = defaultPolicy;
            m_explorer = new NativeMultiWorldTesting::TauFirstExplorerMultiAction<NativeContext>(*GetNativePolicy(), tau);
        }

        virtual void UpdatePolicy(IPolicyMultiAction<Ctx>^ newPolicy)
        {
            this->defaultPolicy = newPolicy;
        }

        virtual void EnableExplore(bool explore)
        {
            m_explorer->Enable_Explore(explore);
        }

		~TauFirstExplorerMultiAction()
		{
			delete m_explorer;
		}

	internal:
		virtual void InvokePolicyCallback(Ctx context, cli::array<UInt32>^ actions, int index) override
		{
            cli::array<UInt32>^ defaultActions = defaultPolicy->ChooseAction(context);

            if (defaultActions == nullptr)
            {
                throw gcnew NullReferenceException("List of actions returned by default policy is null.");
            }
            if (defaultActions->Length < actions->Length)
            {
                throw gcnew InvalidDataException("Number of actions returned by default policy is unexpected. Expected: " + actions->Length + ", Actual: " + defaultActions->Length);
            }

            // TODO: possible to remove the copy by forcing users to fill in a preallocated array instead.
            Array::Copy(defaultActions, actions, actions->Length);
		}

		NativeMultiWorldTesting::TauFirstExplorerMultiAction<NativeContext>* Get()
		{
			return m_explorer;
		}

	private:
		IPolicyMultiAction<Ctx>^ defaultPolicy;
		NativeMultiWorldTesting::TauFirstExplorerMultiAction<NativeContext>* m_explorer;
	};

	/// <summary>
	/// The epsilon greedy exploration class.
	/// </summary>
	/// <remarks>
	/// In some cases, different actions have a different scores, and you
	/// would prefer to choose actions with large scores. Softmax allows 
	/// you to do that.
	/// </remarks>
	/// <typeparam name="Ctx">The Context type.</typeparam>
	generic <class Ctx>
	public ref class SoftmaxExplorerMultiAction : public IExplorerMultiAction<Ctx>, public IConsumeScorer<Ctx>, public ScorerCallback<Ctx>
	{
	public:
		/// <summary>
		/// The constructor is the only public member, because this should be used with the MwtExplorer.
		/// </summary>
		/// <param name="defaultScorer">A function which outputs a score for each action.</param>
		/// <param name="lambda">lambda = 0 implies uniform distribution. Large lambda is equivalent to a max.</param>
		/// <param name="numActions">The number of actions to randomize over.</param>
		SoftmaxExplorerMultiAction(IScorer<Ctx>^ defaultScorer, float lambda, UInt32 numActions)
		{
			this->defaultScorer = defaultScorer;
			m_explorer = new NativeMultiWorldTesting::SoftmaxExplorerMultiAction<NativeContext>(*GetNativeScorer(), lambda, (u32)numActions);
		}

        /// <summary>
        /// Initializes a softmax explorer with variable number of actions.
        /// </summary>
        /// <param name="defaultScorer">A function which outputs a score for each action.</param>
        /// <param name="lambda">lambda = 0 implies uniform distribution. Large lambda is equivalent to a max.</param>
        SoftmaxExplorerMultiAction(IScorer<Ctx>^ defaultScorer, float lambda)
        {
            this->defaultScorer = defaultScorer;
            m_explorer = new NativeMultiWorldTesting::SoftmaxExplorerMultiAction<NativeContext>(*GetNativeScorer(), lambda);
        }

        virtual void UpdateScorer(IScorer<Ctx>^ newScorer)
        {
            this->defaultScorer = newScorer;
        }

        virtual void EnableExplore(bool explore)
        {
            m_explorer->Enable_Explore(explore);
        }

		~SoftmaxExplorerMultiAction()
		{
			delete m_explorer;
		}

	internal:
		virtual List<float>^ InvokeScorerCallback(Ctx context) override
		{
			return defaultScorer->ScoreActions(context);
		}

		NativeMultiWorldTesting::SoftmaxExplorerMultiAction<NativeContext>* Get()
		{
			return m_explorer;
		}

	private:
		IScorer<Ctx>^ defaultScorer;
		NativeMultiWorldTesting::SoftmaxExplorerMultiAction<NativeContext>* m_explorer;
	};

	/// <summary>
	/// The generic exploration class.
	/// </summary>
	/// <remarks>
	/// GenericExplorerMultiAction provides complete flexibility.  You can create any
	/// distribution over actions desired, and it will draw from that.
	/// </remarks>
	/// <typeparam name="Ctx">The Context type.</typeparam>
	generic <class Ctx>
    public ref class GenericExplorerMultiAction : public IExplorerMultiAction<Ctx>, public IConsumeScorer<Ctx>, public ScorerCallback<Ctx>
	{
	public:
		/// <summary>
		/// The constructor is the only public member, because this should be used with the MwtExplorer.
		/// </summary>
		/// <param name="defaultScorer">A function which outputs the probability of each action.</param>
		/// <param name="numActions">The number of actions to randomize over.</param>
		GenericExplorerMultiAction(IScorer<Ctx>^ defaultScorer, UInt32 numActions)
		{
			this->defaultScorer = defaultScorer;
			m_explorer = new NativeMultiWorldTesting::GenericExplorerMultiAction<NativeContext>(*GetNativeScorer(), (u32)numActions);
		}

        /// <summary>
        /// Initializes a generic explorer with variable number of actions.
        /// </summary>
        /// <param name="defaultScorer">A function which outputs the probability of each action.</param>
        GenericExplorerMultiAction(IScorer<Ctx>^ defaultScorer)
        {
            this->defaultScorer = defaultScorer;
            m_explorer = new NativeMultiWorldTesting::GenericExplorerMultiAction<NativeContext>(*GetNativeScorer());
        }

        virtual void UpdateScorer(IScorer<Ctx>^ newScorer)
        {
            this->defaultScorer = newScorer;
        }

        virtual void EnableExplore(bool explore)
        {
            m_explorer->Enable_Explore(explore);
        }

		~GenericExplorerMultiAction()
		{
			delete m_explorer;
		}

	internal:
		virtual List<float>^ InvokeScorerCallback(Ctx context) override
		{
			return defaultScorer->ScoreActions(context);
		}

		NativeMultiWorldTesting::GenericExplorerMultiAction<NativeContext>* Get()
		{
			return m_explorer;
		}

	private:
		IScorer<Ctx>^ defaultScorer;
		NativeMultiWorldTesting::GenericExplorerMultiAction<NativeContext>* m_explorer;
	};

	/// <summary>
	/// The bootstrap exploration class.
	/// </summary>
	/// <remarks>
	/// The Bootstrap explorer randomizes over the actions chosen by a set of
	/// default policies.  This performs well statistically but can be
	/// computationally expensive.
	/// </remarks>
	/// <typeparam name="Ctx">The Context type.</typeparam>
	generic <class Ctx>
	public ref class BootstrapExplorerMultiAction : public IExplorerMultiAction<Ctx>, public IConsumePolicies<Ctx>, public PolicyCallback<Ctx>
	{
	public:
		/// <summary>
		/// The constructor is the only public member, because this should be used with the MwtExplorer.
		/// </summary>
		/// <param name="defaultPolicies">A set of default policies to be uniform random over.</param>
		/// <param name="numActions">The number of actions to randomize over.</param>
		BootstrapExplorerMultiAction(cli::array<IPolicyMultiAction<Ctx>^>^ defaultPolicies, UInt32 numActions)
		{
			this->defaultPolicies = defaultPolicies;
			if (this->defaultPolicies == nullptr)
			{
				throw gcnew ArgumentNullException("The specified array of default policy functions cannot be null.");
			}

			m_explorer = new NativeMultiWorldTesting::BootstrapExplorerMultiAction<NativeContext>(*GetNativePolicies((u32)defaultPolicies->Length), (u32)numActions);
		}

        /// <summary>
        /// Initializes a bootstrap explorer with variable number of actions.
        /// </summary>
        /// <param name="defaultPolicies">A set of default policies to be uniform random over.</param>
        BootstrapExplorerMultiAction(cli::array<IPolicyMultiAction<Ctx>^>^ defaultPolicies)
        {
            this->defaultPolicies = defaultPolicies;
            if (this->defaultPolicies == nullptr)
            {
                throw gcnew ArgumentNullException("The specified array of default policy functions cannot be null.");
            }

            m_explorer = new NativeMultiWorldTesting::BootstrapExplorerMultiAction<NativeContext>(*GetNativePolicies((u32)defaultPolicies->Length));
        }

        virtual void UpdatePolicy(cli::array<IPolicyMultiAction<Ctx>^>^ newPolicies)
        {
            this->defaultPolicies = newPolicies;
        }

        virtual void EnableExplore(bool explore)
        {
            m_explorer->Enable_Explore(explore);
        }

		~BootstrapExplorerMultiAction()
		{
			delete m_explorer;
		}

	internal:
		virtual void InvokePolicyCallback(Ctx context, cli::array<UInt32>^ actions, int index) override
		{
			if (index < 0 || index >= defaultPolicies->Length)
			{
				throw gcnew InvalidDataException("Internal error: Index of interop bag is out of range.");
			}
            cli::array<UInt32>^ defaultActions = defaultPolicies[index]->ChooseAction(context);

            if (defaultActions == nullptr)
            {
                throw gcnew NullReferenceException("List of actions returned by default policy is null.");
            }
            if (defaultActions->Length < actions->Length)
            {
                throw gcnew InvalidDataException("Number of actions returned by default policy is unexpected. Expected: " + actions->Length + ", Actual: " + defaultActions->Length);
            }

            // TODO: possible to remove the copy by forcing users to fill in a preallocated array instead.
            Array::Copy(defaultActions, actions, actions->Length);
		}

		NativeMultiWorldTesting::BootstrapExplorerMultiAction<NativeContext>* Get()
		{
			return m_explorer;
		}

	private:
		cli::array<IPolicyMultiAction<Ctx>^>^ defaultPolicies;
		NativeMultiWorldTesting::BootstrapExplorerMultiAction<NativeContext>* m_explorer;
	};

	/// <summary>
	/// The top level MwtExplorer class.  Using this makes sure that the
	/// right bits are recorded and good random actions are chosen.
	/// </summary>
	/// <typeparam name="Ctx">The Context type.</typeparam>
	generic <class Ctx>
	public ref class MwtExplorer : public RecorderCallback<Ctx>
	{
	public:
        /// <summary>
        /// Constructor.
        /// </summary>
        /// <param name="appId">This should be unique to each experiment to avoid correlation bugs.</param>
        /// <param name="recorder">A user-specified class for recording the appropriate bits for use in evaluation and learning.</param>
        MwtExplorer(String^ appId, IRecorderMultiAction<Ctx>^ recorder) : RecorderCallback(nullptr)
        {
            this->appId = appId;
            this->recorder = recorder;
        }

		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="appId">This should be unique to each experiment to avoid correlation bugs.</param>
		/// <param name="recorder">A user-specified class for recording the appropriate bits for use in evaluation and learning.</param>
        /// <param name="getNumberOfActionsFunc">Optional; A callback function to retrieve the number of actions for the current context.</param>
        MwtExplorer(String^ appId, IRecorderMultiAction<Ctx>^ recorder, Func<Ctx, UInt32>^ getNumberOfActionsFunc) : RecorderCallback(getNumberOfActionsFunc)
		{
			this->appId = appId;
			this->recorder = recorder;
		}

		/// <summary>
		/// Choose_Action should be drop-in replacement for any existing policy function.
		/// </summary>
		/// <param name="explorer">An existing exploration algorithm (one of the above) which uses the default policy as a callback.</param>
		/// <param name="unique_key">A unique identifier for the experimental unit. This could be a user id, a session id, etc...</param>
		/// <param name="context">The context upon which a decision is made. See SimpleContext above for an example.</param>
		/// <returns>An unsigned 32-bit integer representing the 1-based chosen action.</returns>
        cli::array<UInt32>^ ChooseAction(IExplorerMultiAction<Ctx>^ explorer, String^ unique_key, Ctx context)
		{
			String^ salt = this->appId;
			NativeMultiWorldTesting::MwtExplorer<NativeContext> mwt(marshal_as<std::string>(salt), *GetNativeRecorder());

			// Normal handles are sufficient here since native code will only hold references and not access the object's data
			// https://www.microsoftpressstore.com/articles/article.aspx?p=2224054&seqNum=4
			GCHandle selfHandle = GCHandle::Alloc(this);
			IntPtr selfPtr = (IntPtr)selfHandle;

			GCHandle contextHandle = GCHandle::Alloc(context);
			IntPtr contextPtr = (IntPtr)contextHandle;

			GCHandle explorerHandle = GCHandle::Alloc(explorer);
			IntPtr explorerPtr = (IntPtr)explorerHandle;

            cli::array<UInt32>^ actions = nullptr;
            GCHandle actionListHandle;

            try
            {
                NativeContext native_context(selfPtr.ToPointer(), explorerPtr.ToPointer(), contextPtr.ToPointer(),
                    this->GetNumActionsCallback());

                u32 action = 0;
                NativeMultiWorldTesting::IExplorerMultiAction<NativeContext>* native_explorer = nullptr;

                if (explorer->GetType() == EpsilonGreedyExplorerMultiAction<Ctx>::typeid)
                {
                    EpsilonGreedyExplorerMultiAction<Ctx>^ epsilonGreedyExplorerMultiAction = (EpsilonGreedyExplorerMultiAction<Ctx>^)explorer;
                    native_explorer = (NativeMultiWorldTesting::IExplorerMultiAction<NativeContext>*)epsilonGreedyExplorerMultiAction->Get();
                }
                else if (explorer->GetType() == TauFirstExplorerMultiAction<Ctx>::typeid)
                {
                    TauFirstExplorerMultiAction<Ctx>^ tauFirstExplorer = (TauFirstExplorerMultiAction<Ctx>^)explorer;
                    native_explorer = (NativeMultiWorldTesting::IExplorerMultiAction<NativeContext>*)tauFirstExplorer->Get();
                }
                else if (explorer->GetType() == SoftmaxExplorerMultiAction<Ctx>::typeid)
                {
                    SoftmaxExplorerMultiAction<Ctx>^ softmaxExplorer = (SoftmaxExplorerMultiAction<Ctx>^)explorer;
                    native_explorer = (NativeMultiWorldTesting::IExplorerMultiAction<NativeContext>*)softmaxExplorer->Get();
                }
                else if (explorer->GetType() == GenericExplorerMultiAction<Ctx>::typeid)
                {
                    GenericExplorerMultiAction<Ctx>^ genericExplorer = (GenericExplorerMultiAction<Ctx>^)explorer;
                    native_explorer = (NativeMultiWorldTesting::IExplorerMultiAction<NativeContext>*)genericExplorer->Get();
                }
                else if (explorer->GetType() == BootstrapExplorerMultiAction<Ctx>::typeid)
                {
                    BootstrapExplorerMultiAction<Ctx>^ bootstrapExplorer = (BootstrapExplorerMultiAction<Ctx>^)explorer;
                    native_explorer = (NativeMultiWorldTesting::IExplorerMultiAction<NativeContext>*)bootstrapExplorer->Get();
                }

                if (native_explorer == nullptr)
                {
                    throw gcnew Exception("Unknown type of exploration algorithm used.");
                }

                UInt32 numActions = mwt.Get_Number_Of_Actions(*native_explorer, native_context);
                actions = gcnew cli::array<UInt32>(numActions);

                // Get pinned handle to pass through interop boundary and so that native code can modify
                actionListHandle = GCHandle::Alloc(actions, GCHandleType::Pinned);
                IntPtr actionListPtr = (IntPtr)actionListHandle;

                // This is achieved by storing the pointer in the internal context
                native_context.Set_Clr_Action_List(actionListPtr.ToPointer());

                // Conver to native array
                IntPtr actionListPinnedPtr = actionListHandle.AddrOfPinnedObject();
                u32* native_actions = (u32*)actionListPinnedPtr.ToPointer();

                mwt.Choose_Action(*native_explorer, marshal_as<std::string>(unique_key), native_context, native_actions, numActions);
            }
            finally
            {
                if (actionListHandle.IsAllocated)
                {
                    actionListHandle.Free();
                }
                if (explorerHandle.IsAllocated)
                {
                    explorerHandle.Free();
                }
                if (contextHandle.IsAllocated)
                {
                    contextHandle.Free();
                }
                if (selfHandle.IsAllocated)
                {
                    selfHandle.Free();
                }
            }

            return actions;
		}

	internal:
        virtual void InvokeRecorderCallback(Ctx context, cli::array<UInt32>^ actions, float probability, String^ unique_key) override
		{
			recorder->Record(context, actions, probability, unique_key);
		}

	private:
		IRecorderMultiAction<Ctx>^ recorder;
		String^ appId;
	};

	/// <summary>
	/// Represents a feature in a sparse array.
	/// </summary>
	[StructLayout(LayoutKind::Sequential)]
	public value struct Feature
	{
		float Value;
		UInt32 Id;
	};
}

/*! @} End of Doxygen Groups*/
