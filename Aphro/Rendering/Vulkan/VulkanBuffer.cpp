#include "VulkanBuffer.h"

#include <cassert>

namespace aph {

    /**
     * Returns the minimum instance size required to be compatible with devices minOffsetAlignment
     *
     * @param instanceSize The size of an instance
     * @param minOffsetAlignment The minimum required alignment, in bytes, for the offset member (eg
     * minUniformBufferOffsetAlignment)
     *
     * @return VkResult of the buffer mapping call
     */
    VkDeviceSize VulkanBuffer::getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment) {
        if (minOffsetAlignment > 0) {
            return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
        }
        return instanceSize;
    }

    VulkanBuffer::VulkanBuffer(
        VulkanDevice& device,
        VkDeviceSize instanceSize,
        uint32_t instanceCount,
        VkBufferUsageFlags usageFlags,
        VkMemoryPropertyFlags memoryPropertyFlags,
        VkDeviceSize minOffsetAlignment)
        : m_device{ device },
        m_instanceSize{ instanceSize },
        m_instanceCount{ instanceCount },
        m_usageFlags{ usageFlags },
        m_memoryPropertyFlags{ memoryPropertyFlags } {
        m_alignmentSize = getAlignment(instanceSize, minOffsetAlignment);
        m_bufferSize = m_alignmentSize * instanceCount;
        device.createBuffer(m_bufferSize, usageFlags, memoryPropertyFlags, m_buffer, m_memory);
    }

    VulkanBuffer::~VulkanBuffer() {
        unmap();
        vkDestroyBuffer(m_device.device(), m_buffer, nullptr);
        vkFreeMemory(m_device.device(), m_memory, nullptr);
    }

    /**
     * Map a m_memory range of this buffer. If successful, m_mapped points to the specified buffer range.
     *
     * @param size (Optional) Size of the m_memory range to map. Pass VK_WHOLE_SIZE to map the complete
     * buffer range.
     * @param offset (Optional) Byte offset from beginning
     *
     * @return VkResult of the buffer mapping call
     */
    VkResult VulkanBuffer::map(VkDeviceSize size, VkDeviceSize offset) {
        assert(m_buffer && m_memory && "Called map on buffer before create");
        return vkMapMemory(m_device.device(), m_memory, offset, size, 0, &m_mapped);
    }

    /**
     * Unmap a m_mapped m_memory range
     *
     * @note Does not return a result as vkUnmapMemory can't fail
     */
    void VulkanBuffer::unmap() {
        if (m_mapped) {
            vkUnmapMemory(m_device.device(), m_memory);
            m_mapped = nullptr;
        }
    }

    /**
     * Copies the specified data to the m_mapped buffer. Default value writes whole buffer range
     *
     * @param data Pointer to the data to copy
     * @param size (Optional) Size of the data to copy. Pass VK_WHOLE_SIZE to flush the complete buffer
     * range.
     * @param offset (Optional) Byte offset from beginning of m_mapped region
     *
     */
    void VulkanBuffer::writeToBuffer(void* data, VkDeviceSize size, VkDeviceSize offset) {
        assert(m_mapped && "Cannot copy to unmapped buffer");

        if (size == VK_WHOLE_SIZE) {
            memcpy(m_mapped, data, m_bufferSize);
        } else {
            char* memOffset = (char*)m_mapped;
            memOffset += offset;
            memcpy(memOffset, data, size);
        }
    }

    /**
     * Flush a m_memory range of the buffer to make it visible to the device
     *
     * @note Only required for non-coherent m_memory
     *
     * @param size (Optional) Size of the m_memory range to flush. Pass VK_WHOLE_SIZE to flush the
     * complete buffer range.
     * @param offset (Optional) Byte offset from beginning
     *
     * @return VkResult of the flush call
     */
    VkResult VulkanBuffer::flush(VkDeviceSize size, VkDeviceSize offset) {
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = m_memory;
        mappedRange.offset = offset;
        mappedRange.size = size;
        return vkFlushMappedMemoryRanges(m_device.device(), 1, &mappedRange);
    }

    /**
     * Invalidate a m_memory range of the buffer to make it visible to the host
     *
     * @note Only required for non-coherent m_memory
     *
     * @param size (Optional) Size of the m_memory range to invalidate. Pass VK_WHOLE_SIZE to invalidate
     * the complete buffer range.
     * @param offset (Optional) Byte offset from beginning
     *
     * @return VkResult of the invalidate call
     */
    VkResult VulkanBuffer::invalidate(VkDeviceSize size, VkDeviceSize offset) {
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = m_memory;
        mappedRange.offset = offset;
        mappedRange.size = size;
        return vkInvalidateMappedMemoryRanges(m_device.device(), 1, &mappedRange);
    }

    /**
     * Create a buffer info descriptor
     *
     * @param size (Optional) Size of the m_memory range of the descriptor
     * @param offset (Optional) Byte offset from beginning
     *
     * @return VkDescriptorBufferInfo of specified offset and range
     */
    VkDescriptorBufferInfo VulkanBuffer::descriptorInfo(VkDeviceSize size, VkDeviceSize offset) {
        return VkDescriptorBufferInfo{
            m_buffer,
            offset,
            size,
        };
    }

    /**
     * Copies "instanceSize" bytes of data to the m_mapped buffer at an offset of index * alignmentSize
     *
     * @param data Pointer to the data to copy
     * @param index Used in offset calculation
     *
     */
    void VulkanBuffer::writeToIndex(void* data, int index) {
        writeToBuffer(data, m_instanceSize, index * m_alignmentSize);
    }

    /**
     *  Flush the m_memory range at index * alignmentSize of the buffer to make it visible to the device
     *
     * @param index Used in offset calculation
     *
     */
    VkResult VulkanBuffer::flushIndex(int index) { return flush(m_alignmentSize, index * m_alignmentSize); }

    /**
     * Create a buffer info descriptor
     *
     * @param index Specifies the region given by index * alignmentSize
     *
     * @return VkDescriptorBufferInfo for instance at index
     */
    VkDescriptorBufferInfo VulkanBuffer::descriptorInfoForIndex(int index) {
        return descriptorInfo(m_alignmentSize, index * m_alignmentSize);
    }

    /**
     * Invalidate a m_memory range of the buffer to make it visible to the host
     *
     * @note Only required for non-coherent m_memory
     *
     * @param index Specifies the region to invalidate: index * alignmentSize
     *
     * @return VkResult of the invalidate call
     */
    VkResult VulkanBuffer::invalidateIndex(int index) {
        return invalidate(m_alignmentSize, index * m_alignmentSize);
    }

}