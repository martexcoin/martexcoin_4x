#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

class CForge;
extern CForge forgeMain;

class CForge
{
public:
    class CForgeItem
    {
    private:
        std::string name;
        std::string txHash;
        std::string outputIndex;

    public:
        CForgeItem(std::string name, std::string txHash, std::string outputIndex)
        {
            this->name = name;
            this->txHash = txHash;
            this->outputIndex = outputIndex;
        }

        const std::string& getName() const
        {
            return name;
        }

        const std::string& getOutputIndex() const
        {
            return outputIndex;
        }

        bool castOutputIndex(int& n) const;

        void setOutputIndex(const std::string& outputIndex)
        {
            this->outputIndex = outputIndex;
        }

        const std::string& getTxHash() const
        {
            return txHash;
        }

        void setTxHash(const std::string& txHash)
        {
            this->txHash = txHash;
        }
    };

    CForge()
    {
        entries = std::vector<CForgeItem>();
    }

    void clear();
    bool read(std::string& strErr);
    bool readForgeConfig(std::string& strErr);
    CForge::CForgeItem* add(std::string name, std::string txHash, std::string outputIndex);
    void remove(std::string txHash);

    std::vector<CForgeItem>& getEntries()
    {
        return entries;
    }

    int getCount()
    {
        int c = -1;
        for (CForgeItem e : entries) {
            if (e.getTxHash() != "") c++;
        }
        return c;
    }

private:
    std::vector<CForgeItem> entries;
};