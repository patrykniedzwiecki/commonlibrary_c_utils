/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

 /**
 * @file ashmem.h
 *
 * @brief Provides the <b>Ashmem</b> class implemented in c_utils to operate the
 * Anonymous Shared Memory (Ashmem).
 */

#ifndef UTILS_BASE_ASHMEM_H
#define UTILS_BASE_ASHMEM_H

#include <cstddef>
#include <linux/ashmem.h>
#include "refbase.h"
#include "parcel.h"

namespace OHOS {
/**
 * @brief Creates an <b>Ashmem</b> region in the kernel.
 *
 * @param name Indicates the pointer to the name that will be
 * copied and assigned to the <b>Ashmem</b> region in the kernel.
 * @param size Indicates the size of the <b>Ashmem</b> region to create.
 * @return Returns the file descriptor of the <b>Ashmem</b> region.
 */
int AshmemCreate(const char *name, size_t size);

/**
 * @brief Sets the protection flag of an <b>Ashmem</b> region in the kernel.
 *
 * @param fd Indicates the file descriptor of an <b>Ashmem</b> region.
 * @param prot Indicates the value of the protection flag.
 * @return Returns <b>0</b> if the operation is successful;
 * returns <b>-1</b> otherwise.
 */
int AshmemSetProt(int fd, int prot);

/**
 * @brief Obtains the size of a specific <b>Ashmem</b> region in the kernel.
 *
 * @param fd Indicates the file descriptor of an <b>Ashmem</b> region.
 * @return Returns the size of the <b>Ashmem</b> region.
 */
int AshmemGetSize(int fd);

/**
 * @brief Provides the <b>Ashmem</b> class implemented in c_utils to
 * operate the Anonymous Shared Memory (Ashmem).
 *
 * You can use the interfaces in this class to create <b>Ashmem</b> regions
 * and map them to implement write and read operations.
 *
 * @note <b>Ashmem</b> regions should be unmapped and closed manually,
 * though managed by a smart pointer.
 */
class Ashmem : public virtual RefBase {
public:
    /**
     * @brief Creates an <b>Ashmem</b> region in the kernel.
     *
     * The <b>/dev/ashmem</b> file will be opened, whose file
     * descriptor will be held by the created <b>Ashmem</b> region.
     *
     * @param name Indicates the pointer to the name that will be
	 * copied and assigned to the <b>Ashmem</b> region in the kernel.
     * @param size Indicates the size of the <b>Ashmem</b> region.
     * @return Returns the created <b>Ashmem</b> region referenced by a
     * smart pointer.
     * @note Before writing/reading data in the region, use `MapAshmem()`.
     *
     */
    static sptr<Ashmem> CreateAshmem(const char *name, int32_t size);

    /**
     * @brief Closes this <b>Ashmem</b> region (through the file descriptor).
     *
     * All inner parameters will be cleared.
     *
     * @note An <b>Ashmem</b> region will be unmapped by `UnmapAshmem()`
	 * before being closed.
     */
    void CloseAshmem();

    /**
     * @brief Maps this <b>Ashmem</b> region in the kernel to user space.
     *
     * @param mapType Indicates the protection flag of the mapped region in
     * user space.
     * @return Returns <b>true</b> if mapping is successful.
     */
    bool MapAshmem(int mapType);

    /**
     * @brief Maps this <b>Ashmem</b> region in read/write mode.
     *
     * It calls `MapAshmem(PROT_READ | PROT_WRITE)`.
     *
     * @return Returns <b>true</b> if mapping is successful.
     */
    bool MapReadAndWriteAshmem();

    /**
     * @brief Maps this <b>Ashmem</b> region in read-only mode.
     *
     * It calls `MapAshmem(PROT_READ)`.
     *
     * @return Returns <b>true</b> if mapping is successful.
     */
    bool MapReadOnlyAshmem();

    /**
     * @brief Unmaps this <b>Ashmem</b> region.
     *
     * Unmapping works only when the <b>Ashmem</b> region has been mapped.
     * It will clear the protection flag.
     */
    void UnmapAshmem();

    /**
     * @brief Sets the protection flag of this <b>Ashmem</b> region.
     *
     * @param protectionType Indicates the value of the protection flag.
     * @return Returns <b>True</b> if the operation is successful.
     */
    bool SetProtection(int protectionType);

    /**
     * @brief Obtains the protection flag of this <b>Ashmem</b> region.
     *
     * @return Returns the protection flag. For details, see the Linux manual.
     */
    int GetProtection();

    /**
     * @brief Obtains the size of this <b>Ashmem</b> region.
     *
     * @return Returns the size.
     */
    int32_t GetAshmemSize();

    /**
     * @brief Writes data to the `offset` position of this <b>Ashmem</b> region.
     *
     * Bounds and the protection flag will be checked.
     *
     * @param data Indicates the pointer to the data to write.
     * @param size Indicates the size of the data to write, in bytes.
     * @param offset Indicates the offset from the start position of the
	 * <b>Ashmem</b> region.
     * @return Returns <b>True</b> if the operation is successful; returns
     * <b>False</b> if overflow occurs or the protection flag is illegal.
     * @note This operation requires the write permission on both the
     * <b>Ashmem</b> region in the kernel and the mapped region in user space.
     */
    bool WriteToAshmem(const void *data, int32_t size, int32_t offset);

    /**
     * @brief Reads data from the `offset` position of this <b>Ashmem</b> region.
     *
     * Bounds and the protection flag will be checked.
     *
     * @param size Indicates the size of the data to read, in bytes.
     * @param offset Indicates the offset from the start position of
	 * the <b>Ashmem</b> region.
     * @return Returns the void-type pointer to the data. `nullptr` is returned
     * if overflow occurs or the protection flag is illegal.
     * @note This operation requires the read permission on both the
     * <b>Ashmem</b> region in the kernel and the mapped region in user space.
     */
    const void *ReadFromAshmem(int32_t size, int32_t offset);

    /**
     * @brief Creates an <b>Ashmem</b> region.
     *
     * @param fd Indicates the file descriptor of the <b>Ashmem</b> region
	 * in the kenrel.
     * @param size Indicates the size of the <b>Ashmem</b> region.
     */
    Ashmem(int fd, int32_t size);
    ~Ashmem() override;

    /**
     * @brief Obtains the file descriptor of this <b>Ashmem</b> region in the kernel.
     *
     * @return Returns the file descriptor. The value <b>0</b> is returned
     * if the <b>Ashmem</b> region is closed.
     */
    int GetAshmemFd() const
    {
        return memoryFd_;
    };
    
private:
    int memoryFd_; // File descriptor of the Ashmem region.
    int32_t memorySize_; // Size of the Ashmem region.
    int flag_; // Protection flag of the Ashmem region in user space.
    void *startAddr_; // Start address of the Ashmem region.
    bool CheckValid(int32_t size, int32_t offset, int cmd);
};
} // namespace OHOS
#endif
