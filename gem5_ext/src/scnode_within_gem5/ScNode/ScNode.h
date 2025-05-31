class ScNode
{
protected:
    ScNode(const ScNode&) = default;
    ScNode(ScNode&&) = default;
    ScNode& operator=(const ScNode&) = default;
    ScNode& operator=(ScNode&&) = default;

public:
    ScNode() = default;
    virtual ~ScNode() = default;

    virtual void bind(tlm_utils::multi_target_base<>& target) = 0;
    virtual uint64_t totalRequests() = 0;
    virtual uint64_t inflightRequests() = 0;
};
